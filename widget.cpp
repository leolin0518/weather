#include "widget.h"
#include "ui_widget.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

QNetworkRequest network_request;
QNetworkRequest network_request_cityinfo;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();

     /*设置发送数据*/
     manage = new QNetworkAccessManager(this);
     manage_cityInfo = new QNetworkAccessManager(this);
     QString cityName = "北京";
     QString provName = "北京";
     qDebug() << "init cityName:" << cityName;

     //发送请求
     setNetworkRequest(network_request, cityName);
     setNetworkRequestCityInfo(network_request_cityinfo,provName);
     connect(manage,SIGNAL(finished(QNetworkReply *)),this,SLOT(getReplyFinished(QNetworkReply*)));
     connect(manage_cityInfo, SIGNAL(finished(QNetworkReply *)), this, SLOT(getReplyFinishedCityInfo(QNetworkReply*)));

    /*发送get网络请求*/
    manage->get(network_request);
    manage_cityInfo->get(network_request_cityinfo);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    ui_init();
    setWindowTitle(tr("天气预报"));
}

void Widget::ui_init()//界面初始化
{
    ui->dangqian_wendu_label->clear();

    ui->city_comboBox_p->hide();
    ui->city_comboBox_c->hide();
    ui->city_comboBox_a->hide();

    ui->save_pushButton->hide();
    getProvinceList();
}

void Widget::setNetworkRequest(QNetworkRequest &request, QString cityName)
{
    request.setUrl(QUrl(QString("http://apis.baidu.com/apistore/weatherservice/recentweathers?cityname=%1")
                                .arg(cityName)));
    request.setRawHeader("apikey", "b446bb51d329b1098b008568231a772b");
}

void Widget::setNetworkRequestCityInfo(QNetworkRequest &request, QString Name)
{
    request.setUrl(QUrl(QString("http://apis.baidu.com/apistore/weatherservice/citylist?cityname=%1")
                                .arg(Name)));
    request.setRawHeader("apikey", "b446bb51d329b1098b008568231a772b");
}

void Widget::getTodayWeatherInfo(QJsonObject data)
{

    QJsonObject today = data.value("retData").toObject().value("today").toObject();
    todayInfo.currCity = data.value("retData").toObject().value("city").toString();
    qDebug() << "currCity:" << todayInfo.currCity;
    todayInfo.date = today.value("date").toString();//2016-08-26
    todayInfo.week = today.value("week").toString();//星期五
    todayInfo.type = today.value("type").toString();//阵雨
    todayInfo.curTemp = today.value("curTemp").toString();//30℃
    todayInfo.hightemp = today.value("hightemp").toString();//34℃
    todayInfo.lowtemp = today.value("lowtemp").toString();//24℃
    todayInfo.fengli = today.value("fengli").toString();//微风级
    todayInfo.fengxiang = today.value("fengxiang").toString();//东北风
    todayInfo.aqi = today.value("aqi").toString();//60

    qDebug() << "getTodayWeatherInfo:\n" << todayInfo.currCity + todayInfo.date + todayInfo.week << todayInfo.type << todayInfo.curTemp
             << todayInfo.hightemp << todayInfo.lowtemp << todayInfo.fengli << todayInfo.fengxiang
             << todayInfo.aqi;

    QStringList todayInfoList;
    todayInfoList << todayInfo.currCity + todayInfo.date + todayInfo.week << todayInfo.type << todayInfo.curTemp
                  << todayInfo.hightemp << todayInfo.lowtemp << todayInfo.fengli << todayInfo.fengxiang
                  << todayInfo.aqi;

    setUI_information();//设置UI上的信息

}

void Widget::getForecastWeatherInfo(QJsonObject data)
{
    QStringList forecastInfoList;
    QJsonArray forecastArray = data.value("retData").toObject().value("forecast").toArray();
    int size = forecastArray.size();
    qDebug() << "size;" << size;

    for(int i=0; i < size; i++)
    {
        QJsonObject forecast = forecastArray.at(i).toObject();

        forecastInfo.date = forecast.value("date").toString();
        forecastInfo.week = forecast.value("week").toString();
        forecastInfo.type = forecast.value("type").toString();
        forecastInfo.curTemp = forecast.value("curTemp").toString();
        forecastInfo.hightemp = forecast.value("hightemp").toString();
        forecastInfo.lowtemp = forecast.value("lowtemp").toString();
        forecastInfo.fengli = forecast.value("fengli").toString();
        forecastInfo.fengxiang = forecast.value("fengxiang").toString();
        forecastInfo.aqi = forecast.value("aqi").toString();
//        qDebug() << forecastInfo.date + forecastInfo.week << forecastInfo.type << forecastInfo.curTemp
//                 << forecastInfo.hightemp << forecastInfo.lowtemp << forecastInfo.fengli << forecastInfo.fengxiang
//                 << forecastInfo.aqi;

        QString foreInfo = forecastInfo.date + "," + forecastInfo.week + "," +  forecastInfo.type + "," + forecastInfo.type + "," +
                           forecastInfo.hightemp + "," + forecastInfo.lowtemp + "," + forecastInfo.fengli + "," + forecastInfo.fengxiang +"," + forecastInfo.aqi;
        qDebug() << "foreInfo:" << foreInfo;//"2016-09-03,星期六,多云,多云,34℃,25℃,微风级,东风,"
        forecastInfoList << foreInfo;
        forecasetInfo_date << forecastInfo.date;

        qDebug() << "forecastInfoList:" << forecastInfoList;

    }


    for(int i=0, j=1; i < forecastInfoList.size(); i++)
    {
        QString tmp = forecastInfoList.at(i);
        qDebug() << tmp;
       // QString dd = tmp.section(',', 4, 4);//34℃
       // QString ddd = dd.mid(0,2);//mid()函数接受两个参数，第一个是起始位置，第二个是取串的长度。34℃ -> 34
        //QString i_str = QString::number(dd,10);
       // qDebug() << "ddd: " << ddd;
        QString wendu_max =QString::number(j,10) + "," + tmp.section(',', 4, 4).mid(0,2);//34℃ 最高温度
        QString wendu_min =QString::number(j,10) + "," + tmp.section(',', 5, 5).mid(0,2);//14℃ 最低温度
        j = j + 2;
        qDebug() << "wendu_max: " << wendu_max << "wendu_min:" << wendu_min;
       // wendu_max =  i + "," + wendu_max;
        forecastInfo_wenduMax << wendu_max;
        forecastInfo_wenduMin << wendu_min;
    }

    qDebug() << "forecastInfo_wenduMax:" << forecastInfo_wenduMax;//forecastInfo_wenduMax: ("34℃", "33℃", "31℃", "31℃")
    qDebug() << "forecastInfo_wenduMin:" << forecastInfo_wenduMin;

    QStringList set_chart_string;
    set_chart_string << "Max:" << "" << "" << forecastInfo_wenduMax ;//("Min", "", "", "0,20", "1,20", "2,20", "3,18")
    qDebug() << "set_chart_string:" << set_chart_string;

    QStringList set_chart_string_wendu_min;
    set_chart_string_wendu_min << "Min" << "" << "" << forecastInfo_wenduMin ;
    qDebug() << "set_chart_string_wendu_min:" << set_chart_string_wendu_min;

    splineChart(set_chart_string, set_chart_string_wendu_min);
    //splineChart(set_chart_string_wendu_min);
    forecastInfo_wenduMax.clear();
    forecastInfo_wenduMin.clear();

}




void Widget::getOtherInfo(QJsonObject data)
{
    QJsonArray otherArray = data.value("retData").toObject().value("today").toObject().value("index").toArray();

    QString index, details,name;
    QJsonObject otherInfo = otherArray.at(0).toObject();//感冒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(1).toObject();//防晒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details << "name:" <<name;
    ui->fangshaizhishu_label_1->setText(name);
    ui->fangshaizhishu_label_2->setText(index);

    otherInfo = otherArray.at(2).toObject();//穿衣指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(3).toObject();//运动指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->yongdongzhishu_label_5->setText(name);
    ui->yongdongzhishu_label_6->setText(index);

    otherInfo = otherArray.at(4).toObject();//洗车指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->xichezhishu_label_7->setText(name);
    ui->xichezhishu_label_8->setText(index);

    otherInfo = otherArray.at(5).toObject();//晾晒指数
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    name = otherInfo.value("name").toString();
    qDebug() << "details:" << details;
    ui->liangshaizhishu_label_3->setText(name);
    ui->liangshaizhishu_label_4->setText(index);

}

void Widget::refreshWeather(QString str)
{

    setNetworkRequest(network_request, str);
    manage->get(network_request);
}

void Widget::refreshCityInfo(QString str)
{

    setNetworkRequestCityInfo(network_request_cityinfo,str);
    manage_cityInfo->get(network_request_cityinfo);
}


void Widget::getProvinceList()
{
    QStringList proName;
    proName << "北京" << "上海"<< "天津" << "重庆" << "黑龙江" << "吉林" << "辽宁" << "内蒙古" << "河北" << "山西" << "陕西" << "山东" << "新疆" << "西藏" << "青海" << "甘肃" << "宁夏" << "河南" << "江苏" << "湖北" << "浙江" << "安徽" << "福建" << "江西" << "湖南" << "贵州" << "四川" << "广东" << "云南" << "广西" << "海南" << "香港" << "澳门" << "台湾";
    ui->city_comboBox_p->clear();
    ui->city_comboBox_p->addItems(proName);
}

void Widget::getCityList(QJsonObject data)
{
     QStringList cityInfoList;
     cityInfoList.clear();
     QJsonArray cityinfo = data.value("retData").toArray();
     int size = cityinfo.size();
     qDebug() << "cityinfo.......................size;" << size;
     for(int i=0; i < size; i++)
     {
         QJsonObject tmp = cityinfo.at(i).toObject();
         QString province_cn = tmp.value("province_cn").toString();
         if(province_cn == ui->city_comboBox_p->currentText())
         {
             QString district_cn = tmp.value("district_cn").toString();
             cityInfoList << district_cn;
         }

     }
    qDebug() << "777777777777777777" << cityInfoList.removeDuplicates();
    ui->city_comboBox_c->addItems(cityInfoList);

}


void Widget::getAreaList(QJsonObject data)
{
    QStringList areaInfoList;
    areaInfoList.clear();
    ui->city_comboBox_a->clear();
    QJsonArray cityinfo = data.value("retData").toArray();
    int size = cityinfo.size();
    qDebug() << "cityinfo.......................size;" << size;
    for(int i=0; i < size; i++)
    {
        QJsonObject tmp = cityinfo.at(i).toObject();
        QString district_cn = tmp.value("district_cn").toString();
        if(district_cn == ui->city_comboBox_c->currentText())
        {
            QString name_cn = tmp.value("name_cn").toString();
            areaInfoList << name_cn;
        }

    }

   qDebug() << "areaInfoList" << areaInfoList;
   ui->city_comboBox_a->addItems(areaInfoList);

}


void Widget::setUI_information()//设置界面显示信息，如当前温度，空气指数等
{
    ui->currCity_label->clear();
    ui->currCity_label->setText(todayInfo.currCity);//显示当前城市

    ui->dangqian_wendu_label->clear();
    ui->dangqian_wendu_label->setText(todayInfo.curTemp);//显示当前实时温度

    QString dangqian_min_max = tr("温度: ") + todayInfo.lowtemp + " ~ " + todayInfo.hightemp;
    ui->dangqian_min_maxwendu_info_label->setText(dangqian_min_max);//显示当前温度范围

    QString dangqian_date = todayInfo.date + "  " + todayInfo.week;
    ui->dangqian_date_info_label->setText(dangqian_date);//显示当前日期

    ui->dangqian_kongqizhiliang_info_label->setText(tr("实时空气质量:"));
    ui->dangqian_kongqizhiliang_img_label->setText(todayInfo.aqi);//显示当前空气质量
    int aqi = todayInfo.aqi.toInt();
    qDebug() << "aqi:" << aqi;
    if(aqi >=0 && aqi <= 50 )
    {
      ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#51FF00;");
    }
    if(aqi > 50 && aqi <= 100 )
    {
      ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#AFDB00;");
    }
    if(aqi > 100 && aqi <= 200 )
    {
      ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#FF6666;");
    }
    if(aqi > 200)
    {
      ui->dangqian_kongqizhiliang_img_label->setStyleSheet("background-color:#CC0033;");
    }


    //设置天气img start
        if(todayInfo.type == "晴")
        {
            QPixmap tianqi_pixmap(":/img/ico/qing.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        if(todayInfo.type == "阴")
        {
            QPixmap tianqi_pixmap(":/img/ico/yin.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        if(todayInfo.type == "阵雨" || todayInfo.type == "雷阵雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/leizhenyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }

        if(todayInfo.type == "多云")
        {
            QPixmap tianqi_pixmap(":/img/ico/duoyun.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }

        if(todayInfo.type == "小雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/xiaoyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        if(todayInfo.type == "中雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/xiaoyu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        if(todayInfo.type == "大雨")
        {
            QPixmap tianqi_pixmap(":/img/ico/dayu.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        if(todayInfo.type == "undefined")
        {
            QPixmap tianqi_pixmap(":/img/ico/undefined.png");
            //qDebug() << tianqi_pixmap.width() << " /" << tianqi_pixmap.height();
            tianqi_pixmap = tianqi_pixmap.scaled(ui->dangqian_tianqi_img_label->width(), ui->dangqian_tianqi_img_label->height(),
                                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致
            ui->dangqian_tianqi_img_label->setPixmap(tianqi_pixmap);
            ui->dangqian_tianqi_img_label->show();
        }
        //设置天气img end
}

void Widget::splineChart(QStringList maxList, QStringList minList)
{

    int x_max = 0, x_min = 100, y_max = 0, y_min = 100;
    qDebug() << "maxList:" << maxList << maxList.size();
    qDebug() << "minList:" << minList << minList.size();

    QSplineSeries *seriesMax = new QSplineSeries();//曲线 //new QLineSeries();//折线
    QSplineSeries *seriesMin = new QSplineSeries();//曲线

    seriesMax->setName(maxList.at(0));//设置曲线Max的标题内容
    seriesMin->setName(minList.at(0));//设置曲线Min的标题内容
    seriesMax->setPen(QPen(Qt::red,2,Qt::SolidLine));//设置曲线颜色宽度
    seriesMin->setPen(QPen(Qt::blue,2,Qt::SolidLine));

    for(int i=3; i < maxList.size(); i++)//Max曲线上添加点坐标
    {
      QString tmp = maxList.at(i);
      qDebug() << tmp;
      int value_p = tmp.section(',', 0, 0).toInt();//"5,8" int:  5
      int value_l = tmp.section(',', 1, 1).toInt();//"5,8" int:  8
      x_max = qMax(x_max, value_p);
      x_min = qMin(x_min, value_p);
      y_max = qMax(y_max, value_l);
      y_min = qMin(y_min, value_l);
      qDebug() << "Max->int: " << value_p << "," << value_l;
      seriesMax->append(value_p, value_l);
    }


    for(int i=3; i < minList.size(); i++)//Min曲线上添加点坐标
    {
      QString tmp = minList.at(i);
      qDebug() << tmp;
      int value_p = tmp.section(',', 0, 0).toInt();//"5,8" int:  5
      int value_l = tmp.section(',', 1, 1).toInt();//"5,8" int:  8
      x_max = qMax(x_max, value_p);
      x_min = qMin(x_min, value_p);
      y_max = qMax(y_max, value_l);
      y_min = qMin(y_min, value_l);
      qDebug() << "Min->int: " << value_p << "," << value_l;
      seriesMin->append(value_p, value_l);
    }


    qDebug() << "x_max: " << x_max << ",x_min" << x_min;//x轴上对应的点的最大值和最小值
    qDebug() << "y_max: " << y_max << ",y_min" << y_min;//y轴上对应的点的最大值和最小值



    QCategoryAxis *axisX = new QCategoryAxis();
    QValueAxis *axisY = new QValueAxis;//http://blog.csdn.net/linbounconstraint/article/details/52440807

    //自定义XY轴上显示的label的颜色 Customize axis label colors
    //QBrush axisBrush(Qt::black);
    //axisX->setLabelsBrush(axisBrush);
    //axisY->setLabelsBrush(axisBrush);
    //axisX->setLabelFormat("%g"); //设置刻度的格式
    //axisX->setTitleText(""); //设置X轴的标题
    //axisX->setGridLineVisible(true); //设置是否显示网格线
    //axisX->setMinorTickCount(4); //设置小刻度线的数目
    //axisX->setLabelsVisible(false); //设置刻度是否显示

    /*
        设置X轴上label的显示内容和范围
        ("a", 2), 	("b", 4), 	("c", 6),	("d", 8),
        Pairt(1,20) Pairt(3,20) Pairt(5,20) Pairt(7,20)
        //    axisX->append("a", 1);
    */
    if(!forecasetInfo_date.isEmpty())
    {
        for(int j = 1; j <= forecasetInfo_date.size(); j++)
        {
            axisX->append(forecasetInfo_date.at(j-1), j*2);//axisX->append("a", 1);
        }
        qDebug() << "[leo]forecasetInfo_date.size():" << forecasetInfo_date.size();
        axisX->setRange(0,  forecasetInfo_date.size()*2);
    }
    forecasetInfo_date.clear();



    axisY->setRange(y_min - 1 , y_max + 1);
    axisY->setLabelFormat("%d");
    //axisY->setGridLineVisible(true);//设置刻度是否显示


    QChart *chart = new QChart();

    chart->addSeries(seriesMax);//把曲线Max添加到chart上
    chart->addSeries(seriesMin);//把曲线Min添加到chart上
    chart->setTitle(tr("未来四天温度走势图"));
    //chart->setAnimationOptions(QChart::AllAnimations);//设置曲线呈动画显示
    //chart->createDefaultAxes();//创建曲线的轴 默认值
    chart->setAxisX(axisX, seriesMax);
    chart->setAxisY(axisY, seriesMax);
    chart->setAxisX(axisX, seriesMin);
    chart->setAxisY(axisY, seriesMin);


    if(maxList.at(0) != "")//valueList的第一个字符内容，如果内容为空则隐藏legend，否则显示字符内容为标题
    {
      chart->legend()->show();
    }
    else
    {
      chart->legend()->hide();
    }


//    QString axisX_str = maxList.at(1);//设置X轴的范围
//    if(axisX_str != "")
//    {
//      qDebug() << axisX_str;
//      int axisX_p = axisX_str.section(',', 0, 0).toInt();//"5,8" int:  5
//      int axisX_l = axisX_str.section(',', 1, 1).toInt();//"5,8" int:  8
//      chart->axisX()->setRange(axisX_p, axisX_l);//设置X轴的范围，如果不设置，将默认取当前线段上的点的最大x,y的值作为最大range
//    }

//    QString axisY_str = maxList.at(2);//设置Y轴的范围
//    if(axisY_str != "")
//    {
//      qDebug() << axisY_str;
//      int axisY_p = axisY_str.section(',', 0, 0).toInt();//"5,8" int:  5
//      int axisY_l = axisY_str.section(',', 1, 1).toInt();//"5,8" int:  8
//      chart->axisY()->setRange(axisY_p, axisY_l);//设置Y轴的范围，如果不设置，将默认取当前线段上的点的最大x,y的值作为最大range
//    }


    ui->chart_widget->setRenderHint(QPainter::Antialiasing);//防止曲线出现“锯齿”现象
    ui->chart_widget->setChart(chart);

}



void Widget::getReplyFinished(QNetworkReply *reply)//获取天气api传回的数据
{
    QJsonObject json_data = QJsonDocument::fromJson(reply->readAll()).object();
    //qDebug() << "Json 天气信息:" << json_data;

    //获取历史天气信息
    //getHistoryWeatherInfo(json_data);

    //获取当日天气信息
    getTodayWeatherInfo(json_data);

    //获取未来天气信息
    getForecastWeatherInfo(json_data);

    //获取其他天气信息
    getOtherInfo(json_data);


}

void Widget::getReplyFinishedCityInfo(QNetworkReply *reply)
{
    QJsonObject json_citydata = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << "Json 天气信息:" << json_citydata;

    //getProvinceList();
    getCityList(json_citydata);
    getAreaList(json_citydata);

}



void Widget::on_comboBox_currentIndexChanged(const QString &arg1)
{
    qDebug() << arg1;
    refreshWeather(arg1);
}


//显示City选项
void Widget::on_showSetting_pushButton_clicked()
{

    ui->save_pushButton->show();
    ui->showSetting_pushButton->hide();

    ui->city_comboBox_p->show();
    ui->city_comboBox_c->show();
    ui->city_comboBox_a->show();
}


void Widget::on_save_pushButton_clicked()
{
    refreshWeather(ui->city_comboBox_a->currentText());

    ui->save_pushButton->hide();
    ui->city_comboBox_p->hide();
    ui->city_comboBox_c->hide();
    ui->city_comboBox_a->hide();

    ui->showSetting_pushButton->show();
    ui->currCity_label->setText(todayInfo.currCity);
}


void Widget::on_city_comboBox_p_activated(const QString &arg1)
{
    ui->city_comboBox_c->clear();
    refreshCityInfo(ui->city_comboBox_p->currentText());
}

void Widget::on_city_comboBox_c_activated(const QString &arg1)
{
     refreshCityInfo(ui->city_comboBox_c->currentText());
}
