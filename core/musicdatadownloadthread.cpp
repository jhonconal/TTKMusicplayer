#include "musicdatadownloadthread.h"

MusicDataDownloadThread::MusicDataDownloadThread(const QString &url, const QString &save,
                                                       QObject *parent)
    : MusicDownLoadThreadAbstract(url, save, parent)
{

}

void MusicDataDownloadThread::startToDownload()
{
    if( !m_file->exists() || m_file->size() < 4 )
    {
        if( m_file->open(QIODevice::WriteOnly) )
        {
            m_manager = new QNetworkAccessManager(this);
            startRequest(m_url);
        }
        else
        {
            qDebug() <<"The data file create failed";
            emit musicDownLoadFinished("The data file create failed");
            deleteAll();
        }
    }
}

void MusicDataDownloadThread::startRequest(const QUrl &url)
{
    m_reply = m_manager->get( QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()), this, SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     SLOT(replyError(QNetworkReply::NetworkError)) );
    connect(m_reply, SIGNAL(readyRead()),this, SLOT(downLoadReadyRead()));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)),
                     SIGNAL(downloadProgress(qint64,qint64)));
}

void MusicDataDownloadThread::downLoadFinished()
{
    if(!m_file)
    {
        return;
    }

    m_file->flush();
    m_file->close();
    QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(m_reply->error())
    {
        m_file->remove();
    }
    else if(!redirectionTarget.isNull())
    {
//       QUrl newUrl = reply->url().resolved(redirectionTarget.toUrl());
        m_reply->deleteLater();
        m_file->open(QIODevice::WriteOnly);
        m_file->resize(0);
        startRequest(m_reply->url().resolved(redirectionTarget.toUrl()));
        return;
    }
    else
    {
        emit musicDownLoadFinished("Music");
        qDebug()<<"data download has finished!";
        deleteAll();
    }
}

void MusicDataDownloadThread::downLoadReadyRead()
{
    if(m_file)
      m_file->write(m_reply->readAll());
}
