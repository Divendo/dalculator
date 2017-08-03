#include "updatechecker.h"
#include <QUrl>
#include <QApplication>
#include <QClipboard>

namespace updates
{
    // Classes:
        // version:
            // Public:
                version::version(const ushort& major, const ushort& minor, const ushort& revision, const ushort& build, const bool& beta)
                :_major(major), _minor(minor), _revision(revision), _build(build), _beta(beta){}
                version::version(const QString& str)
                {fromString(str);}

                bool version::fromString(const QString& str)
                {
                    if(str.size()<7 || str.count('.')!=3)
                        return false;
                    QString buffer="";
                    QString::const_iterator pos=str.begin();
                    bool ok=true;

                    // Major:
                        for(; pos!=str.end(); pos++)
                        {
                            if(*pos>='0' && *pos<='9')
                                buffer+=*pos;
                            else if(*pos=='.')
                                break;
                            else
                                return false;
                        }
                        _major=buffer.toUShort(&ok);
                        if(!ok) return false;
                        buffer="";

                    // Minor:
                        for(pos++; pos!=str.end(); pos++)
                        {
                            if(*pos>='0' && *pos<='9')
                                buffer+=*pos;
                            else if(*pos=='.')
                                break;
                            else
                                return false;
                        }
                        _minor=buffer.toUShort(&ok);
                        if(!ok) return false;
                        buffer="";

                    // Revision:
                        for(pos++; pos!=str.end(); pos++)
                        {
                            if(*pos>='0' && *pos<='9')
                                buffer+=*pos;
                            else if(*pos=='.')
                                break;
                            else
                                return false;
                        }
                        _revision=buffer.toUShort(&ok);
                        if(!ok) return false;
                        buffer="";

                    // Build:
                        for(pos++; pos!=str.end(); pos++)
                        {
                            if(*pos>='0' && *pos<='9')
                                buffer+=*pos;
                            else if(*pos==' ')
                                break;
                            else
                                return false;
                        }
                        _build=buffer.toUShort(&ok);
                        if(!ok) return false;
                        buffer="";

                    // Beta:
                        if(pos!=str.end() && (pos++)!=str.end())
                            _beta=(*pos=='1');

                    return ok;
                }

                QString version::toString() const
                {
                    QString out="", tmp="";
                    out+=tmp.setNum(_major)+'.';
                    out+=tmp.setNum(_minor)+'.';
                    out+=tmp.setNum(_revision)+'.';
                    out+=tmp.setNum(_build);
                    return out;
                }

                version& version::setMajor(const ushort& newMajor)
                {_major=newMajor;return *this;}
                ushort& version::major()
                {return _major;}
                ushort version::major() const
                {return _major;}

                version& version::setMinor(const ushort& newMinor)
                {_minor=newMinor;return *this;}
                ushort& version::minor()
                {return _minor;}
                ushort version::minor() const
                {return _minor;}

                version& version::setRevision(const ushort& newRevision)
                {_revision=newRevision;return *this;}
                ushort& version::revision()
                {return _revision;}
                ushort version::revision() const
                {return _revision;}

                version& version::setBuild(const ushort& newBuild)
                {_build=newBuild;return *this;}
                ushort& version::build()
                {return _build;}
                ushort version::build() const
                {return _build;}

                version& version::setBeta(const bool& newBeta)
                {_beta=newBeta; return *this;}
                bool& version::beta()
                {return _beta;}
                bool version::beta() const
                {return _beta;}

                /*void version::print() const
                {
                    std::cout<<_major<<'.'<<_minor<<'.'<<_revision<<'.'<<_build<<'\n';
                    std::flush(std::cout);
                }*/

                bool version::operator<(const version& other) const
                {
                    if(_major<other._major)
                        return true;
                    if(_major==other._major)
                    {
                        if(_minor<other._minor)
                            return true;
                        if(_minor==other._minor)
                        {
                            if(_revision<other._revision)
                                return true;
                            if(_revision==other._revision)
                            {
                                if(_build<other._build)
                                    return true;
                                if(_build==other._build && _beta && !other._beta)
                                    return true;
                            }
                        }
                    }
                    return false;
                }
                bool version::operator>(const version& other) const
                {
                    if(_major>other._major)
                        return true;
                    if(_major==other._major)
                    {
                        if(_minor>other._minor)
                            return true;
                        if(_minor==other._minor)
                        {
                            if(_revision>other._revision)
                                return true;
                            if(_revision==other._revision)
                            {
                                if(_build>other._build)
                                    return true;
                                if(_build==other._build && !_beta && other._beta)
                                    return true;
                            }
                        }
                    }
                    return false;
                }
                bool version::operator==(const version& other) const
                {return (_major==other._major && _minor==other._minor && _revision==other._revision && _build==other._build && _beta==other._beta);}
                bool version::operator<=(const version& other) const
                {return (operator==(other) || operator<(other));}
                bool version::operator>=(const version& other) const
                {return (operator==(other) || operator>(other));}

        // updateChecker:
            // Public:
                // We use HTTP for version checking since Qt seems to have trouble with OpenSSL
                updateChecker::updateChecker(const version& initVersion)
                :reply(0), currVersion(initVersion), updateUrl("http://markkamsma.nl/update-checker.php?app=dalculator"),
                softwarePage("https://markkamsma.nl/portfolio/dalculator")
                {
                    connect(this, SIGNAL(readSucces(const QString&)), this, SLOT(newVersionAvailableTranslator(const QString&)));
                    networkAccess.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
                }

                bool updateChecker::isChecking() const
                {return reply!=0;}
                version updateChecker::getVersion() const
                {return currVersion;}
                QUrl updateChecker::getUpdateUrl() const
                {return updateUrl;}
                QUrl updateChecker::getSoftwarePage() const
                {return softwarePage;}
            // Public slots:
                bool updateChecker::checkForUpdates()
                {
                    if(reply!=0)
                        return false;
                    reply=networkAccess.get(QNetworkRequest(updateUrl));
                    connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
                    return true;
                }

                updateChecker& updateChecker::setVersion(const version& newVersion)
                {currVersion=newVersion;return *this;}

                bool updateChecker::newVersionAvailable(const version& compareWith) const
                {
                    if(compareWith>currVersion)
                    {
                        updatesChecked(true);
                        return true;
                    }
                    else
                    {
                        updatesChecked(false);
                        return false;
                    }
                }

            // Private slots:
                void updateChecker::requestFinished()
                {
                    if(reply->error() != QNetworkReply::NoError)
                        errorOccurred(true);
                    else
                    {
                        QByteArray result=reply->readAll();
                        if(result.size()<7)                 // Het moeten minstens 4 cijfers gescheiden door punten zijn (bv: 1.0.0.0)
                            errorOccurred(false);
                        else
                            readSucces(result);
                    }
                    reply=0;
                }

                void updateChecker::newVersionAvailableTranslator(const QString& str) const
                {
                    version tmp;
                    if(tmp.fromString(str))
                        newVersionAvailable(tmp);
                    else
                        errorOccurred(false);
                }
}
