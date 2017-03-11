#ifndef FS_STAT_FILE_SYSTEM_SCANNER_H
#define FS_STAT_FILE_SYSTEM_SCANNER_H

#include <condition_variable>
#include <experimental/filesystem>
#include <map>
#include <mutex>
#include <string>
#include <thread>

namespace fs_stat {

	struct file_group_statistics {
		file_group_statistics(std::uint32_t count = 0, std::uintmax_t size = 0) :
			count(count),
			size(size) { }

		std::uint32_t count;
		std::uintmax_t size;
	};

	struct statistics {
		statistics(std::map<std::string, file_group_statistics> file_statistics, std::uint32_t subdirectories_count) :
			file_statistics(file_statistics),
			subdirectories_count(subdirectories_count) { }

		std::map<std::string, file_group_statistics> file_statistics;
		std::uint32_t subdirectories_count;
	};

	class file_system_scanner {
		public:
			file_system_scanner();
			file_system_scanner(const file_system_scanner&) = delete;
			//file_system_scanner(file_system_scanner&&);

			~file_system_scanner();

			file_system_scanner& operator=(const file_system_scanner&) = delete;

			void scan(const std::experimental::filesystem::path& dir_path);
			void stop();
			bool scan_is_finished();
			// returns statistics that has been gathered
			// since the last invoñation of this method
			statistics capture_statistics();

		private:
			void clear_statistics();
			std::uint32_t get_subdirectories_count(const std::experimental::filesystem::path& dir_path);
			void gather_statistics(const std::experimental::filesystem::path& dir_path);
			void run_gatherer();

		private:
			std::thread gathering_thread;

			// a path to the directory to scan
			// is a part of the start command data
			std::experimental::filesystem::path directory;

			// a flag used to notify the gatherer thread that the start command arrived
			// is a part of the start command data
			bool start_requested;
			// a flag used to notify the gatherer thread that the exit command arrived
			// is a part of the exit command data
			bool exit_requested;
			// a mutex for the start and exit commands' data
			std::mutex cmd_mtx;
			// a signal that wakes the gatherer thread for scanning start or execution termination
			std::condition_variable cmd_signal;

			// a flag used to notify the gatherer thread that the stop command arrived
			// is a part of the stop command data
			bool stop_requested;
			// a mutex for the stop command data
			std::mutex stop_mtx;

			// a flag used to notify the clients that the scanning process is finished
			bool is_finished;
			std::mutex finished_mtx;

			std::map<std::string, file_group_statistics> file_statistics;
			std::uint32_t subdirectories_count;
			std::mutex statistics_mtx;
	};

}

#endif // FS_STAT_FILE_SYSTEM_SCANNER_H
