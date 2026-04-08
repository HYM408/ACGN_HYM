#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <QJsonObject>

struct SubjectsData {
    int subjectId = 0;
    int subjectType = 0;
    QString name;
    QString nameCn;
    QString summary;
    QJsonObject tags;
    QString metaTags;
    int subjectEps = 0;
    int subjectVolumes = 0;
    double ratingScore = 0.0;
    int ratingRank = 0;
    QString date;
    QVector<int> scoreDetails;
    int doing = 0;
    int collect = 0;
    int dropped = 0;
    int onHold = 0;
    int wish = 0;
    int epStatus = 0;
    int volStatus = 0;
    int type = 0;
    int rate = 0;
};

struct EpisodeData {
    int subjectId = 0;
    int episodeId = 0;
    double ep = 0.0;
    double sort = 0.0;
    QString name;
    QString nameCn;
    int episodeType = 0;
    int collectionType = 0;
};

struct PersonInfo {
    int personId = 0;
    QString personName;
    QString personNameCn;
};

struct CharacterData {
    int characterId = 0;
    QString characterName;
    QString characterNameCn;
    int type = 0;
    QVector<PersonInfo> persons;
};

struct PersonData {
    int personId = 0;
    int position = 0;
    QString name;
    QString nameCn;
};

struct SubjectRelationData {
    SubjectsData subject;
    int relationType = 0;
};

struct GameData {
    int subjectId = 0;
    QString launchPath;
    QString savePath;
    int playDuration = 0;
};

Q_DECLARE_METATYPE(EpisodeData)

#endif // DATA_STRUCTS_H