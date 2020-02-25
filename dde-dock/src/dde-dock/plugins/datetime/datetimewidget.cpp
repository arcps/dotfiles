/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "datetimewidget.h"
#include "constants.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QSvgRenderer>
#include <QMouseEvent>

#define PLUGIN_STATE_KEY    "enable"

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent)
{

}

void DatetimeWidget::set24HourFormat(const bool value)
{
    if (m_24HourFormat == value) {
        return;
    }

    m_24HourFormat = value;

    m_cachedTime.clear();
    update();

    if (isVisible()) {
        emit requestUpdateGeometry();
    }
}

QSize DatetimeWidget::sizeHint() const
{
    QFontMetrics fm(qApp->font());

    if (m_24HourFormat)
        return fm.boundingRect("88:88").size() + QSize(20, 10);
    else
        return fm.boundingRect("88:88 A.A.").size() + QSize(20, 20);
}

void DatetimeWidget::resizeEvent(QResizeEvent *e)
{
    m_cachedTime.clear();

    QWidget::resizeEvent(e);
}

void DatetimeWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    const auto ratio = devicePixelRatioF();
    const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    const QDateTime current = QDateTime::currentDateTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (displayMode == Dock::Efficient)
    {
        QString format;
        if (m_24HourFormat)
            format = "hh:mm";
        else
        {
            if (position == Dock::Top || position == Dock::Bottom)
                format = "hh:mm AP";
            else
                format = "hh:mm\nAP";
        }

        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, current.time().toString(format));
        return;
    }

    QString format = "hh:mm";
    if (!m_24HourFormat && (position == Dock::Top || position == Dock::Bottom)) {
        format = "hh:mm AP";
    }

    int fontSizeTime, fontSizeDate;
    fontSizeDate = std::min(width(), height()) * 0.24;
    if (m_24HourFormat) 
        fontSizeTime = std::min(width(), height()) * 0.29;
    else 
        if (position == Dock::Left || position == Dock::Right) {
            fontSizeTime = std::min(width(), height()) * 0.25;
            fontSizeDate = std::min(width(), height()) * 0.23;
        }
        else
            fontSizeTime = std::min(width(), height()) * 0.23;


    
    QFont font = qApp->font();
    painter.setPen(Qt::white);
    QString time = current.toString(format);
    QString dayAndMonth = current.toString("dd/MM");
    QString dayOfWeek = current.toString("ddd");
    QString dayPeriod = current.toString("AP");
    QFontMetrics fontTimeMetric(font);
    QFontMetrics fontDateMetric(font);

    int padding = (height()-fontSizeTime-fontSizeDate*2)/3;
    int line = padding+(fontSizeTime+fontSizeDate)/2;
    int halfWidth = width()/2;
    char str[5];
    sprintf(str,"%d",line);

    
    if (position == Dock::Left || position == Dock::Right) {
        if (!m_24HourFormat) {
            line = line-padding/2;
            font.setPixelSize(fontSizeTime);
            font.setWeight(QFont::Black);
            painter.setFont(font);
            painter.drawText(
                QRectF(
                    halfWidth-fontDateMetric.width(dayPeriod)/2,
                    line*2,
                    fontDateMetric.width(dayPeriod),
                    fontSizeDate),
                Qt::AlignCenter,
                dayPeriod);
        }
        font.setPixelSize(fontSizeDate);
        font.setWeight(QFont::Normal);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontDateMetric.width(dayOfWeek)/2,
                (m_24HourFormat?padding:0),
                fontDateMetric.width(dayOfWeek),
                fontSizeDate),
            Qt::AlignCenter,
            dayOfWeek);

        font.setPixelSize(fontSizeTime);
        font.setWeight(QFont::Black);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontTimeMetric.width(time)/2,
                (m_24HourFormat?line*1:line*1),
                fontTimeMetric.width(time),
                fontSizeTime),
            Qt::AlignCenter,
            time);

        font.setPixelSize(fontSizeDate);
        font.setWeight(QFont::Normal);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontDateMetric.width(dayAndMonth)/2,
                (m_24HourFormat?line*2:line*3),
                fontDateMetric.width(dayAndMonth),
                fontSizeDate),
            Qt::AlignCenter,
            dayAndMonth);
    } else {
        font.setPixelSize(fontSizeDate);
        font.setWeight(QFont::Normal);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontDateMetric.width(dayOfWeek)/2,
                (m_24HourFormat?padding:padding),
                fontDateMetric.width(dayOfWeek),
                fontSizeDate),
            Qt::AlignCenter,
            dayOfWeek);

        font.setPixelSize(fontSizeTime);
        font.setWeight(QFont::Black);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontTimeMetric.width(time)/2,
                (m_24HourFormat?line*1:line*1.2),
                fontTimeMetric.width(time),
                fontSizeTime),
            Qt::AlignCenter,
            time);
            
        font.setPixelSize(fontSizeDate);
        font.setWeight(QFont::Normal);
        painter.setFont(font);
        painter.drawText(
            QRectF(
                halfWidth-fontDateMetric.width(dayAndMonth)/2,
                (m_24HourFormat?line*2:line*2.15),
                fontDateMetric.width(dayAndMonth),
                fontSizeDate),
            Qt::AlignCenter,
            dayAndMonth);
    }

    
    return;
}

const QPixmap DatetimeWidget::loadSvg(const QString &fileName, const QSize size)
{
    const auto ratio = devicePixelRatioF();

    QPixmap pixmap(size * ratio);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}
