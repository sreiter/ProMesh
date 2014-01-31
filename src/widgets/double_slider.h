// created by Sebastian Reiter
// s.b.reiter@gmail.com
// Dec 16, 2013

#ifndef __H__UG__double_slider__
#define __H__UG__double_slider__

#include <QWidget>

class QSlider;

///	The DoubleSlider is similar to QSlider, but provides double values.
/**
 * - Use setRange to specify a minimum and maximum value.
 * - The Resolution (default is 10000) specifies how many separate values are
 * 		represented on the slider. This value normally doesn't have to be changed.
 * - singleStep tells how far the slider moves if one presses right on the keybard.*/
class DoubleSlider : public QWidget
{
	Q_OBJECT

	public:
		DoubleSlider(QWidget* parent = 0);
		virtual ~DoubleSlider();

		void setRange(double min, double max);
		void setResolution(int resolution);

		double value() const;

		void setSingleStep(double singleStep);
		double singleStep() const;

		double minimum() const;
		double maximum() const;

	signals:
		void valueChanged(double value);

	public slots:
		void setValue(double val);

	protected slots:
		void sliderValueChanged(int value);

	private:
		QSlider*	m_slider;
		double m_value;
		double m_min;
		double m_max;
		double m_resolution;
		double m_singleStep;
};

#endif
