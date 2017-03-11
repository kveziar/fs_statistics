#include "StatisticsModel.h"
#include <cassert>
#include "formatters.h"

using namespace fs_stat;

StatisticsModel::StatisticsModel(QObject* parent) :
	QAbstractListModel(parent) { }

QVariant StatisticsModel::data(const QModelIndex& index, int role) const {
	assert((index.row() >= 0) && (index.row() < this->file_groups.size()));
	const std::string extension = this->file_groups[index.row()];

	std::map<std::string, file_group_statistics>::const_iterator file_group{ this->file_statistics.find(extension) };
	assert(file_group != this->file_statistics.cend());

	const fs_stat::file_group_statistics& file_group_stat{ file_group->second };

	switch (role) {
		case Roles::AverageSizeRole: {
			return QVariant(formatSizeValue(file_group_stat.size / file_group_stat.count));
		}
		case Roles::CountRole: {
			return QVariant(file_group_stat.count);
		}
		case Roles::ExtensionRole:
		case Qt::DisplayRole: {
			return QVariant(extension == "" ? "<no extension>" : extension.c_str());
		}
		case Roles::TotalSizeRole: {
			return QVariant(formatSizeValue(file_group_stat.size));
		}
		default: {
			return QVariant();
		}
	}
}

QHash<int, QByteArray> StatisticsModel::roleNames() const {
	QHash<int, QByteArray> roles;

	roles[Roles::AverageSizeRole] = "average_size";
	roles[Roles::CountRole] = "count";
	roles[Roles::ExtensionRole] = "extension";
	roles[Roles::TotalSizeRole] = "total_size";

	return roles;
}

int StatisticsModel::rowCount(const QModelIndex& parent) const {
	return static_cast<int>(this->file_statistics.size());
}

void StatisticsModel::clearStatistics() {
	this->beginResetModel();
	this->file_groups.clear();
	this->file_statistics.clear();
	this->endResetModel();
}

void StatisticsModel::mergeStatistics(const std::map<std::string, file_group_statistics>& diff) {
	for (const std::map<std::string, file_group_statistics>::value_type& file_group : diff) {
		const std::string extension = file_group.first;

		std::pair<std::map<std::string, file_group_statistics>::iterator, bool> insert_result{ this->file_statistics.insert(file_group) };
		if (insert_result.second) {
			this->update_file_group_list(extension);
		} else {
			this->file_statistics[extension].count += file_group.second.count;
			this->file_statistics[extension].size += file_group.second.size;

			std::vector<std::string>::iterator ext_it{ std::find(
				this->file_groups.begin(),
				this->file_groups.end(),
				extension
			) };
			this->dataChanged(
				this->index(ext_it - this->file_groups.begin()),
				this->index(ext_it - this->file_groups.begin()),
				QVector<int>{ Roles::AverageSizeRole, Roles::CountRole, Roles::TotalSizeRole }
			);
		}
	}
}

void StatisticsModel::update_file_group_list(const std::string& extension) {
	std::vector<std::string>::iterator successor{ std::find_if(
		this->file_groups.begin(),
		this->file_groups.end(),
		[extension](const std::string& str) -> bool { return str > extension; }
	) };

	this->beginInsertRows(
		QModelIndex(),
		successor - this->file_groups.begin(),
		successor - this->file_groups.begin()
	);
	this->file_groups.insert(successor, extension);
	this->endInsertRows();
}
