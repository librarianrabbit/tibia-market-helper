#include "localized_spin_box.h"

localized_spin_box::localized_spin_box(QWidget* parent) :
    QSpinBox(parent)
{
}

QString localized_spin_box::textFromValue(int value) const
{
    return this->locale().toString(value);
}
