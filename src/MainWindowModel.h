#ifndef FS_STAT_MAIN_WINDOW_MODEL_H
#define FS_STAT_MAIN_WINDOW_MODEL_H

#include <QFileSystemModel>
#include <QObject>
#include <QTimer>
#include "StatisticsModel.h"
#include "file_system_scanner.h"

namespace fs_stat {

	class MainWindowModel : public QObject {
		Q_OBJECT

		Q_PROPERTY(
			QString directoryPath
			READ directoryPath
			WRITE setDirectoryPath
			NOTIFY directoryPathChanged
		)

		Q_PROPERTY(
			QFileSystemModel* fileSystemModel
			READ fileSystemModel
			NOTIFY fileSystemModelChanged
		)

		Q_PROPERTY(
			StatisticsModel* statisticsModel
			READ statisticsModel
			NOTIFY statisticsModelChanged
		)

		Q_PROPERTY(
			QString errorText
			READ errorText
			NOTIFY errorTextChanged
		)

		Q_PROPERTY(
			bool isScanning
			READ isScanning
			NOTIFY isScanningChanged
		)

		Q_PROPERTY(
			unsigned int subdirectoriesCount
			READ subdirectoriesCount
			NOTIFY subdirectoriesCountChanged
		)

		Q_PROPERTY(
			unsigned long long fileCount
			READ fileCount
			NOTIFY fileCountChanged
		)

		Q_PROPERTY(
			QString fileSizeString
			READ fileSizeString
			NOTIFY fileSizeStringChanged
		)

		signals:
			void directoryPathChanged(const QString& newPath);
			void fileSystemModelChanged(QFileSystemModel* newModel);
			void statisticsModelChanged(StatisticsModel* newModel);
			void errorTextChanged(QString newValue);
			void isScanningChanged(bool newValue);
			void subdirectoriesCountChanged(std::uint32_t newValue);
			void fileCountChanged(unsigned long long newValue);
			void fileSizeStringChanged(QString newValue);

		public:
			explicit MainWindowModel(QObject* parent = nullptr);
			MainWindowModel(const MainWindowModel&) = delete;
			MainWindowModel(MainWindowModel&&) = delete;

			~MainWindowModel() = default;

			MainWindowModel& operator=(const MainWindowModel&) = delete;

			QString directoryPath() const { return this->dir_path; }
			QFileSystemModel* fileSystemModel() const { return this->fs_model; }
			StatisticsModel* statisticsModel() const { return this->stat_model; }
			QString errorText() const { return this->error_text; }
			bool isScanning() const { return this->is_scanning; }
			std::uint32_t subdirectoriesCount() const { return this->subdirectories_count; }
			std::uint64_t fileCount() const { return this->file_count; }
			QString fileSizeString() const;
			std::size_t fileSize() const { return this->file_size; }

		public slots:
			void setDirectoryPath(const QString& path);
			void startStopScan();

		private slots:
			void processScannerUpdates();

		private:
			QString dir_path;
			QTimer* timer;
			QFileSystemModel* fs_model;
			StatisticsModel* stat_model;
			QString error_text;
			bool is_scanning;
			std::uint32_t subdirectories_count;
			std::uint64_t file_count;
			std::size_t file_size;
			fs_stat::file_system_scanner scanner;
	};

}

#endif // FS_STAT_MAIN_WINDOW_MODEL_H
