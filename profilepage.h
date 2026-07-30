#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

class ProfilePage : public QWidget

{
	Q_OBJECT

public:
	explicit ProfilePage(QWidget* parent = nullptr);

private slots:
	void toggleEditMode();
	void saveChanges();

private:
	void setFieldsEditable(bool editable);
	
	QLineEdit* usernameField;
	QLineEdit* emailField;

	QPushButton* editButton;
	QPushButton* saveButton;

	bool isEditing = false;
};
#endif   
