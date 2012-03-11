#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace updates
{
    class version
    {
        public:
            typedef unsigned short ushort;

            version(const ushort& major=1, const ushort& minor=0, const ushort& revision=0, const ushort& build=0, const bool& beta=0);
            version(const QString& str);

            bool fromString(const QString& str);
            QString toString() const;

            version& setMajor(const ushort& newMajor);
            ushort& major();
            ushort major() const;

            version& setMinor(const ushort& newMinor);
            ushort& minor();
            ushort minor() const;

            version& setRevision(const ushort& newRevision);
            ushort& revision();
            ushort revision() const;

            version& setBuild(const ushort& newBuild);
            ushort& build();
            ushort build() const;

            version& setBeta(const bool& newBeta);
            bool& beta();
            bool beta() const;

            // void print() const;

            bool operator<(const version& other) const;
            bool operator>(const version& other) const;
            bool operator==(const version& other) const;
            bool operator<=(const version& other) const;
            bool operator>=(const version& other) const;

        protected:
            ushort _major, _minor, _revision, _build;
            bool _beta;
    };

    class updateChecker : public QObject
    {
        Q_OBJECT

        public:
            updateChecker(const version& initVersion=version());

            bool isChecking() const;
            version getVersion() const;
            QUrl getUpdateUrl() const;
            QUrl getSoftwarePage() const;

        public slots:
            bool checkForUpdates();
            updateChecker& setVersion(const version& newVersion);
            bool newVersionAvailable(const version& compareWith) const;

        signals:
            void updatesChecked(const bool& newVersionAvailable) const;
            void readSucces(const QString& str) const;
            void errorOccurred(const bool& httpError) const;

        private slots:
            void requestFinished();
            void newVersionAvailableTranslator(const QString& str) const;

        private:
            QNetworkAccessManager networkAccess;
            QNetworkReply* reply;
            version currVersion;
            QUrl updateUrl;
            QUrl softwarePage;
    };
}

#endif // UPDATECHECKER_H
