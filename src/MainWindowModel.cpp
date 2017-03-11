#include "MainWindowModel.h"
#include "formatters.h"

using namespace fs_stat;

MainWindowModel::MainWindowModel(QObject* parent):
	QObject(parent),
	dir_path(),
	timer(new QTimer(this)),
	fs_model(new QFileSystemModel(this)),
	stat_model(new StatisticsModel(this)),
	error_text(),
	is_scanning(false),
	subdirectories_count(0),
	file_count(0),
	file_size(0),
	scanner() {
	this->fs_model->setRootPath(QString());
	timer->setInterval(100);
	timer->setSingleShot(false);
	connect(timer, &QTimer::timeout, this, &MainWindowModel::processScannerUpdates);
}

QString MainWindowModel::fileSizeString() const {
	return formatSizeValue(this->file_size);
}

void MainWindowModel::setDirectoryPath(const QString& path) {
	if (this->dir_path != path) {
		this->dir_path = path;
		emit directoryPathChanged(path);
	}
}

void MainWindowModel::startStopScan() {
	if (!this->is_scanning) {
		this->stat_model->clearStatistics();

		this->subdirectories_count = 0;
		emit subdirectoriesCountChanged(this->subdirectories_count);

		this->file_count = 0;
		emit fileCountChanged(this->file_count);

		this->file_size = 0;
		emit fileSizeStringChanged(formatSizeValue(this->file_size));

		try {
			this->scanner.scan(std::experimental::filesystem::u8path(this->dir_path.toStdString()));
			this->error_text = QString();
			emit errorTextChanged(this->error_text);

			this->is_scanning = true;
			emit isScanningChanged(this->is_scanning);

			this->timer->start();
		} catch (const std::invalid_argument&) {
			this->error_text = QString::fromUtf8("specified path is not a directory");
			emit errorTextChanged(this->error_text);
		}
	} else {
		this->scanner.stop();
	}
}

void MainWindowModel::processScannerUpdates() {
	const fs_stat::statistics statistics = this->scanner.capture_statistics();
	this->stat_model->mergeStatistics(statistics.file_statistics);

	for (const std::map<std::string, file_group_statistics>::value_type& file_group : statistics.file_statistics) {
		if (file_group.second.count > 0) {
			this->file_count += file_group.second.count;
			emit fileCountChanged(this->file_count);
		}
		if (file_group.second.size > 0) {
			this->file_size += file_group.second.size;
			emit fileSizeStringChanged(formatSizeValue(this->file_size));
		}
	}

	if (statistics.subdirectories_count > 0) {
		this->subdirectories_count += statistics.subdirectories_count;
		emit subdirectoriesCountChanged(this->subdirectories_count);
	}

	if (this->scanner.scan_is_finished()) {
		this->timer->stop();

		this->is_scanning = false;
		emit isScanningChanged(this->is_scanning);
	}
}
