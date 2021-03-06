#include <QString>
#include <QtTest>

#define private public

#include "contouralgorithms.h"
#include "math_utils.h"
#include "Functors.h"

#include <cmath>

namespace
{
    bool operator ==(const Contour& i_one, const Contour& i_second)
    {
        if(i_one.IsClosed() && i_second.IsClosed() && i_one.GetContourPoints().size() ==
                i_second.GetContourPoints().size())
        {
            for(int i = 0; i<i_one.GetContourPoints().size(); ++i)
                if(i_one.GetContourPoints().indexOf(i_second.GetContourPoints()[i]) == -1)
                    return false;

            return true;
        }

        return i_one.GetContourPoints() == i_second.GetContourPoints() && i_one.IsClosed() == i_second.IsClosed();
    }
}

class Test : public QObject
{
    Q_OBJECT
    
public:
    Test();
    
private Q_SLOTS:
    void LinesToContoursTest();
    void LinesToContoursTest_data();

    void LinesToTLinkedPointsTest();
    void LinesToTLinkedPointsTest_data();

    void TLinkedPointsToContoursTest();
    void TLinkedPointsToContoursTest_data();

    void CombineContourTest();
    void CombineContourTest_data();

    void FormIndexesTest();
    void FormIndexesTest_data();

    void GaussTest();
    void GaussTest_data();

    void FormGaussCoeffsTest();
    void FromGradusToRadianTest();
    void FromRadianToGradusTest();

    void LineAngleInGradusTest();
    void LineAngleInGradusTest_data();

    void DoesLinesWithSameAngleTest();
    void DoesLinesWithSameAngleTest_data();

    void DistanceTest();
    void DistanceTest_data();

    void DistanceFunctorTest();
    void DistanceFunctorTest_data();
};

Test::Test()
{
}


void Test::LinesToContoursTest()
{
    QFETCH(Lines, lines);
    QFETCH(TContours, result);

    QCOMPARE(ContourAlgorithms::LinesToContours(lines), result);
}

void Test::LinesToContoursTest_data()
{
    QTest::addColumn<Lines>("lines");
    QTest::addColumn<TContours>("result");

    Lines lines;
    lines.m_lines.push_back(QLine(0,0,0,1));
    lines.m_lines.push_back(QLine(0,1,1,1));
    lines.m_lines.push_back(QLine(1,1,1,0));
    lines.m_lines.push_back(QLine(1,0,0,0));

    TContours contours;
    Contour contour;
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(0, 1));
    contour.AddPoint(QPoint(1, 1));
    contour.AddPoint(QPoint(1, 0));
    contour.MakeClosed();
    contours.push_back(contour);

    QTest::newRow("1")<<lines<<contours;
}

void Test::LinesToTLinkedPointsTest()
{
    QFETCH(Lines, lines);
    QFETCH(TLinkedPoints, result);

    QCOMPARE(ContourAlgorithms::_LinesToLinkedPoints(lines), result);
}

void Test::LinesToTLinkedPointsTest_data()
{
    QTest::addColumn<Lines>("lines");
    QTest::addColumn<TLinkedPoints>("result");

    Lines lines;
    lines.m_lines.push_back(QLine(0,0,0,1));
    lines.m_lines.push_back(QLine(0,1,1,1));
    lines.m_lines.push_back(QLine(1,1,1,0));
    lines.m_lines.push_back(QLine(1,0,0,0));

    TLinkedPoints linked_points;
    linked_points.insert(QPoint(0, 0), qMakePair(QPoint(1, 0), QPoint(0, 1)));
    linked_points.insert(QPoint(0, 1), qMakePair(QPoint(0, 0), QPoint(1, 1)));
    linked_points.insert(QPoint(1, 1), qMakePair(QPoint(0, 1), QPoint(1, 0)));
    linked_points.insert(QPoint(1, 0), qMakePair(QPoint(1, 1), QPoint(0, 0)));

    QTest::newRow("1")<<lines<<linked_points;
}

void Test::TLinkedPointsToContoursTest()
{
    QFETCH(TLinkedPoints, linked_points);
    QFETCH(TContours, result);

    QCOMPARE(ContourAlgorithms::_LinkedPointsToContours(linked_points), result);
}

void Test::TLinkedPointsToContoursTest_data()
{
    QTest::addColumn<TLinkedPoints>("linked_points");
    QTest::addColumn<TContours>("result");

    TLinkedPoints linked_points;
    linked_points.insert(QPoint(0, 0), qMakePair(QPoint(1, 0), QPoint(0, 1)));
    linked_points.insert(QPoint(0, 1), qMakePair(QPoint(0, 0), QPoint(1, 1)));
    linked_points.insert(QPoint(1, 1), qMakePair(QPoint(0, 1), QPoint(1, 0)));
    linked_points.insert(QPoint(1, 0), qMakePair(QPoint(1, 1), QPoint(0, 0)));

    TContours contours;
    Contour contour;
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(0, 1));
    contour.AddPoint(QPoint(1, 1));
    contour.AddPoint(QPoint(1, 0));
    contour.MakeClosed();
    contours.push_back(contour);

    QTest::newRow("1")<<linked_points<<contours;

    linked_points.insert(QPoint(2, 0), qMakePair(QPoint(3, 0), QPoint(2, 1)));
    linked_points.insert(QPoint(2, 1), qMakePair(QPoint(2, 0), QPoint(3, 1)));
    linked_points.insert(QPoint(3, 1), qMakePair(QPoint(2, 1), QPoint(3, 0)));
    linked_points.insert(QPoint(3, 0), qMakePair(QPoint(3, 1), QPoint(2, 0)));

    contour.Clear();
    contour.AddPoint(QPoint(2, 0));
    contour.AddPoint(QPoint(2, 1));
    contour.AddPoint(QPoint(3, 1));
    contour.AddPoint(QPoint(3, 0));
    contour.MakeClosed();
    contours.push_back(contour);

    QTest::newRow("2")<<linked_points<<contours;
}

void Test::CombineContourTest()
{
    QFETCH(Contour, contour);
    QFETCH(Contour, result);

    const AngleFunctor* p_functor = new AngleFunctor(0);
    ContourAlgorithms::_CombineLinesInContour(contour, p_functor);
    delete p_functor;

    QCOMPARE(contour, result);
}

void Test::CombineContourTest_data()
{
    QTest::addColumn<Contour>("contour");
    QTest::addColumn<Contour>("result");

    Contour contour;
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(1, 0));
    contour.AddPoint(QPoint(2, 0));

    Contour result;
    result.AddPoint(QPoint(0, 0));
    result.AddPoint(QPoint(2, 0));

    QTest::newRow("1")<<contour<<result;

    contour.Clear();
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(0, 1));
    contour.AddPoint(QPoint(1, 1));
    contour.AddPoint(QPoint(1, 0));

    QTest::newRow("2")<<contour<<contour;
    contour.MakeClosed();

    QTest::newRow("3")<<contour<<contour;

    contour.Clear();
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(0, 1));
    contour.AddPoint(QPoint(0, 2));
    contour.AddPoint(QPoint(1, 2));
    contour.AddPoint(QPoint(2, 2));
    contour.AddPoint(QPoint(2, 1));
    contour.AddPoint(QPoint(2, 0));
    contour.AddPoint(QPoint(1, 0));
    contour.MakeClosed();

    result.Clear();
    result.AddPoint(QPoint(0, 0));
    result.AddPoint(QPoint(0, 2));
    result.AddPoint(QPoint(2, 2));
    result.AddPoint(QPoint(2, 0));
    result.MakeClosed();

    QTest::newRow("4")<<contour<<result;

    contour.MakeUnclosed();
    result.MakeUnclosed();
    result.AddPoint(QPoint(1, 0));

    QTest::newRow("5")<<contour<<result;

    contour.Clear();
    contour.AddPoint(QPoint(0, 0));
    contour.AddPoint(QPoint(0, 1));
    contour.AddPoint(QPoint(1, 2));
    contour.AddPoint(QPoint(2, 3));
    contour.AddPoint(QPoint(2, 2));
    contour.AddPoint(QPoint(2, 1));
    contour.AddPoint(QPoint(2, 0));
    contour.AddPoint(QPoint(1, 0));
    contour.MakeClosed();

    result.Clear();
    result.AddPoint(QPoint(0, 0));
    result.AddPoint(QPoint(0, 1));
    result.AddPoint(QPoint(2, 3));
    result.AddPoint(QPoint(2, 0));
    result.MakeClosed();

    QTest::newRow("6")<<contour<<result;
}

void Test::FormIndexesTest()
{
    QFETCH(int, number);
    QFETCH(QVector<int>, result);

    QCOMPARE(Math::FormIndexes(number), result);
}

void Test::FormIndexesTest_data()
{
    QTest::addColumn<int>("number");
    QTest::addColumn<QVector<int> >("result");

    QVector<int> result(1, 0);

    QTest::newRow("1")<<1<<result;

    result.push_front(-1);
    result.push_front(-2);
    result.push_back(1);
    result.push_back(2);

    QTest::newRow("2")<<5<<result;
}

void Test::GaussTest()
{
    QFETCH(double, deviation);
    QFETCH(double, x);
    QFETCH(double, result);

    bool res = Math::double_equal(Math::_Gauss(deviation, x), result);
    QCOMPARE(res, true);
}

void Test::GaussTest_data()
{
    QTest::addColumn<double>("deviation");
    QTest::addColumn<double>("x");
    QTest::addColumn<double>("result");

    QTest::newRow("1")<<1.0<<0.0<<0.3989;
    QTest::newRow("2")<<0.5<<1.0<<0.108;
}

void Test::FormGaussCoeffsTest()
{
    QVector<double> coeffs(Math::FormGaussCoeffs(0.5, 5));

    for(int i = 0; i<coeffs.size()/2; ++i)
    {
        QVERIFY(Math::double_equal(coeffs[i], coeffs[coeffs.size()-i-1]));
    }

    double sum = 0;

    for(int i = 0; i<coeffs.size(); ++i)
        sum += coeffs[i];

    QVERIFY(Math::double_equal(sum, 1.0));
}

void Test::FromGradusToRadianTest()
{
    QCOMPARE(Math::double_equal(Math::FromGradusToRadian(30), 0.5236), true);
    QCOMPARE(Math::double_equal(Math::FromGradusToRadian(170), 2.9671), true);
}

void Test::FromRadianToGradusTest()
{
    QCOMPARE(Math::double_equal(Math::FromRadianToGradus(0.5236), 30), true);
    QCOMPARE(Math::double_equal(Math::FromRadianToGradus(2.9671), 170), true);
}

void Test::LineAngleInGradusTest()
{
    QFETCH(QPoint, a);
    QFETCH(QPoint, b);
    QFETCH(double, result);

    QCOMPARE(Math::double_equal(Math::LineAngleInGradus(a, b), result), true);
}

void Test::LineAngleInGradusTest_data()
{
    QTest::addColumn<QPoint>("a");
    QTest::addColumn<QPoint>("b");
    QTest::addColumn<double>("result");

    QTest::newRow("1")<<QPoint(0, 0)<<QPoint(1, 1)<<45.0;
    QTest::newRow("2")<<QPoint(0, 0)<<QPoint(1, 0)<<0.0;
    QTest::newRow("3")<<QPoint(0, 0)<<QPoint(0, 1)<<90.0;
    QTest::newRow("4")<<QPoint(1, 0)<<QPoint(-1, 3)<<-56.3099;
}

void Test::DoesLinesWithSameAngleTest()
{
    QFETCH(QPoint, a);
    QFETCH(QPoint, b);
    QFETCH(QPoint, c);
    QFETCH(double, deviation);
    QFETCH(bool, result);

    QCOMPARE(Math::DoesLinesWithSameAngle(a, b, c, deviation), result);
}

void Test::DoesLinesWithSameAngleTest_data()
{
    QTest::addColumn<QPoint>("a");
    QTest::addColumn<QPoint>("b");
    QTest::addColumn<QPoint>("c");
    QTest::addColumn<double>("deviation");
    QTest::addColumn<bool>("result");

    QTest::newRow("1")<<QPoint(0, 0)<<QPoint(1, 1)<<QPoint(2, 2)<<0.0<<true;
    QTest::newRow("2")<<QPoint(0, 0)<<QPoint(1, 2)<<QPoint(2, 2)<<40.0<<false;
    QTest::newRow("3")<<QPoint(0, 0)<<QPoint(1, 2)<<QPoint(2, 3)<<40.0<<true;
    QTest::newRow("4")<<QPoint(0, 0)<<QPoint(1, 2)<<QPoint(2, 3)<<15.0<<false;
    QTest::newRow("5")<<QPoint(0, 0)<<QPoint(1, 2)<<QPoint(2, 3)<<20.0<<true;
}

void Test::DistanceTest()
{
    QFETCH(QPoint, a);
    QFETCH(QPoint, b);
    QFETCH(double, result);

    QCOMPARE(Math::double_equal(Math::Distance(a, b), result), true);
}

void Test::DistanceTest_data()
{
    QTest::addColumn<QPoint>("a");
    QTest::addColumn<QPoint>("b");
    QTest::addColumn<double>("result");

    QTest::newRow("1")<<QPoint(0, 0)<<QPoint(1, 0)<<1.0;
    QTest::newRow("2")<<QPoint(0, 0)<<QPoint(0, 0)<<0.0;
    QTest::newRow("3")<<QPoint(0, 0)<<QPoint(1, 1)<<1.4142;
    QTest::newRow("4")<<QPoint(-1, 3)<<QPoint(0, 0)<<3.1622;
}

void Test::DistanceFunctorTest()
{
    QFETCH(QPoint, a);
    QFETCH(QPoint, b);
    QFETCH(QPoint, c);
    QFETCH(double, distance);
    QFETCH(bool, result);

    LengthFunctor functor(distance);

    QCOMPARE(functor(a, b, c), result);
}

void Test::DistanceFunctorTest_data()
{
    QTest::addColumn<QPoint>("a");
    QTest::addColumn<QPoint>("b");
    QTest::addColumn<QPoint>("c");
    QTest::addColumn<double>("distance");
    QTest::addColumn<bool>("result");

    QTest::newRow("1")<<QPoint(0, 0)<<QPoint(1, 0)<<QPoint(2, 0)<<1.0<<true;
    QTest::newRow("2")<<QPoint(0, 0)<<QPoint(2, 0)<<QPoint(4, 0)<<1.0<<false;
    QTest::newRow("3")<<QPoint(0, 0)<<QPoint(2, 0)<<QPoint(3, 0)<<1.0<<true;
    QTest::newRow("4")<<QPoint(0, 0)<<QPoint(1, 1)<<QPoint(2, 2)<<1.0<<false;
    QTest::newRow("5")<<QPoint(0, 0)<<QPoint(2, 0)<<QPoint(5, 0)<<2.0<<true;
}

#undef public

QTEST_APPLESS_MAIN(Test)

#include "test.moc"






