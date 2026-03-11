#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <QJsonObject>

struct CollectionData {
    int subject_id = 0;
    int vol_status = 0;
    int ep_status = 0;
    int subject_type = 0;
    int type = 0;
    int rate = 0;
    QString subject_date;
    QString subject_name;
    QString subject_name_cn;
    int subject_eps = 0;
    int subject_volumes = 0;
    QString subject_images_common;
};

struct SubjectsData {
    int subject_id = 0;
    QString name;
    QString name_cn;
    QString summary;
    QJsonObject tags;
    QString meta_tags;
    int volumes = 0;
    int eps = 0;
    double rating_score = 0.0;
    int rating_rank = 0;
    QString date;
    int rating_total = 0;
    int doing = 0;
    int collect = 0;
    int dropped = 0;
    int on_hold = 0;
    int wish = 0;
};

struct EpisodeData {
    int subject_id = 0;
    int id = 0;
    double ep = 0.0;
    double sort = 0.0;
    QString name;
    QString name_cn;
    int episode_type = 0;
    int collection_type = 0;
    QString created_at;
};

struct PersonInfo {
    int person_id = 0;
    QString person_name;
    QString person_name_cn;
};

struct CharacterData {
    int character_id = 0;
    QString character_name;
    QString character_name_cn;
    int type = 0;
    QVector<PersonInfo> persons;
};

Q_DECLARE_METATYPE(EpisodeData)

#endif // DATA_STRUCTS_H