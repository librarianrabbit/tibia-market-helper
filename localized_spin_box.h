#ifndef LOCALIZED_SPIN_BOX_H
#define LOCALIZED_SPIN_BOX_H

#include <QSpinBox>

class localized_spin_box : public QSpinBox
{
    Q_OBJECT

public:
    explicit localized_spin_box(QWidget* parent = 0);

protected:
    QString textFromValue(int value) const;
};

#endif // LOCALIZED_SPIN_BOX_H
