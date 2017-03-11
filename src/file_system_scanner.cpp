#include "file_system_scanner.h"
#include <chrono>
#include <stack>

using namespace fs_stat;
using namespace std::literals::chrono_literals;
namespace fs = std::experimental::filesystem;

using path = fs::path;
using dir_entry = fs::directory_entry;
using dir_iterator = fs::directory_iterator;
using file_status = fs::file_status;
using file_type = fs::file_type;

file_system_scanner::file_system_scanner() :
	gathering_thread(),
	directory(),
	start_requested(false),
	exit_requested(false),
	cmd_mtx(),
	cmd_signal(),
	stop_requested(false),
	stop_mtx(),
	is_finished(true),
	finished_mtx(),
	file_statistics(),
	subdirectories_count(0),
	statistics_mtx() {
	this->gathering_thread = std::thread{ &file_system_scanner::run_gatherer, this };
}

file_system_scanner::~file_system_scanner() {
	// set the exit command
	{
		std::lock_guard<std::mutex> lock{ this->cmd_mtx };
		this->exit_requested = true;
	}

	this->cmd_signal.notify_one();

	this->gathering_thread.join();
}

void file_system_scanner::scan(const std::experimental::filesystem::path& dir_path) {
	if (!fs::is_directory(dir_path)) {
		throw std::invalid_argument("given path is not a directory");
	}

	// set the start command
	{
		std::lock_guard<std::mutex> lock{ this->cmd_mtx };
		this->start_requested = true;
		this->directory = dir_path;
	}

	this->cmd_signal.notify_one();
}

void file_system_scanner::stop() {
	// set the stop command
	{
		std::lock_guard<std::mutex> lock{ this->stop_mtx };
		this->stop_requested = true;
		this->directory = path{ };
	}
}

bool file_system_scanner::scan_is_finished() {
	std::lock_guard<std::mutex> lock_finished{ this->finished_mtx };
	return this->is_finished;
}

statistics file_system_scanner::capture_statistics() {
	std::lock_guard<std::mutex> lock{ this->statistics_mtx };

	statistics result{ this->file_statistics, this->subdirectories_count };
	this->clear_statistics();

	return result;
}

void file_system_scanner::clear_statistics() {
	this->file_statistics.clear();
	this->subdirectories_count = 0;
}

std::uint32_t file_system_scanner::get_subdirectories_count(const std::experimental::filesystem::path& dir_path) {
	std::uint32_t subdirectories_count{ 0 };

	if (!fs::is_directory(dir_path)) {
		throw std::invalid_argument("given path is not a directory");
	}

	dir_iterator dir_iter{ dir_path };

	for (const dir_iterator::value_type& dir_entry : dir_iter) {
		if (fs::is_directory(dir_entry.status())) {
			subdirectories_count++;
		}
	}

	return subdirectories_count;
}

void file_system_scanner::gather_statistics(const path& dir_path) {
	this->clear_statistics();

	std::stack<path> dirs_to_process;
	dirs_to_process.push(dir_path);

	std::uint32_t subdirectories_count{ this->get_subdirectories_count(dir_path) };

	{
		std::lock_guard<std::mutex> lock{ this->statistics_mtx };
		this->subdirectories_count = subdirectories_count;
	}

	while (!dirs_to_process.empty()) {
		{
			std::lock_guard<std::mutex> lock{ this->stop_mtx };
			if (this->stop_requested) {
				// consume the stop command
				this->stop_requested = false;
				break;
			}
		}

		{
			std::lock_guard<std::mutex> lock{ this->cmd_mtx };
			if (this->exit_requested) {
				// the exit command must be executed but must not be consumed
				// start termination process
				break;
			}
		}

		dir_iterator current_dir_iter{ dirs_to_process.top() };
		dirs_to_process.pop();

		std::map<std::string, file_group_statistics> current_dir_stat;

		for (const dir_iterator::value_type& current_dir_entry : current_dir_iter) {
			try {
				file_status current_dir_entry_status{ current_dir_entry.status() };

				if (fs::is_directory(current_dir_entry_status)) {
					dirs_to_process.push(current_dir_entry);
				} else if (fs::is_regular_file(current_dir_entry_status)) {
					std::string extension;

					if (fs::is_symlink(current_dir_entry.symlink_status())) {
						// process the actual file extension
						const path sm_file{ fs::read_symlink(current_dir_entry) };
						extension = sm_file.extension().string();
					} else {
						extension = current_dir_entry.path().extension().string();
					}

					const uintmax_t file_size = fs::file_size(current_dir_entry);
					current_dir_stat[extension].count++;
					current_dir_stat[extension].size += file_size;
				}
				// skip block and character special files,
				// pipes, socket files and unknown files
			} catch (const fs::filesystem_error&) {
				// skip this directory entry
			}
		}

		{
			std::lock_guard<std::mutex> lock{ this->statistics_mtx };
			for (const std::map<std::string, file_group_statistics>::value_type file_group : current_dir_stat) {
				this->file_statistics[file_group.first].count += file_group.second.count;
				this->file_statistics[file_group.first].size += file_group.second.size;
			}
		}
	}
}

void file_system_scanner::run_gatherer() {
	while (true) {
		std::unique_lock<std::mutex> lock_cmd{ this->cmd_mtx };
		bool* p_start{ &(this->start_requested) };
		bool* p_exit{ &(this->exit_requested) };
		this->cmd_signal.wait(lock_cmd, [p_start, p_exit] { return *p_start || *p_exit; });

		if (this->start_requested) {
			// consume the start command
			this->start_requested = false;
			path directory = this->directory;
			this->directory = path{ };

			{
				std::lock_guard<std::mutex> lock_finished{ this->finished_mtx };
				this->is_finished = false;
			}

			lock_cmd.unlock();
			this->gather_statistics(directory);

			{
				std::lock_guard<std::mutex> lock_finished{ this->finished_mtx };
				this->is_finished = true;
			}
		}

		// execute the exit command
		if (this->exit_requested) {
			return;
		}
	}
}
