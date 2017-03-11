#ifndef FS_STAT_STATISTICS_MODEL_H
#define FS_STAT_STATISTICS_MODEL_H

#include <map>
#include <string>
#include <vector>
#include <QAbstractListModel>
#include "file_system_scanner.h"

namespace fs_stat {

	// a class representing a model for file groups and file groups' statistics
	// to visualize them in the TableView
	class StatisticsModel : public QAbstractListModel {
		Q_OBJECT

		public:
			enum Roles {
				AverageSizeRole = Qt::UserRole + 1,
				CountRole,
				ExtensionRole,
				TotalSizeRole
			};

		public:
			explicit StatisticsModel(QObject* parent = nullptr);
			StatisticsModel(const StatisticsModel&) = delete;
			StatisticsModel(StatisticsModel&&) = delete;

			~StatisticsModel() = default;

			StatisticsModel& operator=(const StatisticsModel&) = delete;

			virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
			virtual QHash<int, QByteArray> roleNames() const override;
			virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
			void clearStatistics();
			// merges file groups' statistics currently stored in the model
			// with the incoming update diff
			void mergeStatistics(const std::map<std::string, file_group_statistics>& diff);

		private:
			void update_file_group_list(const std::string& extension);

		private:
			// a mapping between model indices and file groups
			// (i.e., the display order of groups),
			// files are grouped by extension
			std::vector<std::string> file_groups;

			std::map<std::string, file_group_statistics> file_statistics;
	};

}

#endif // FS_STAT_STATISTICS_MODEL_H
