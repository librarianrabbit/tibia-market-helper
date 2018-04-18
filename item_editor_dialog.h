#ifndef ITEM_EDITOR_DIALOG_H
#define ITEM_EDITOR_DIALOG_H

#include <QDialog>

namespace Ui
{
    class item_editor_dialog;
}

class item_info;

class item_editor_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit item_editor_dialog(QWidget* parent = 0);
    ~item_editor_dialog();

    void from(const item_info& i);
    item_info get_item() const;

    void set_name(const QString& name);

protected:
    void done(int r) override;

protected:
    Ui::item_editor_dialog* ui;
};

#endif // ITEM_EDITOR_DIALOG_H
