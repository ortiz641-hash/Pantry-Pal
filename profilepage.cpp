#include "profilepage.h"

#include <QLabel> 
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

ProfilePage::ProfilePage(QWidget* parent) : QWidget(parent), // Constructor for profile page

usernameField(new QLineEdit(this)), // Textboxes for our profile page 
emailField(new QLineEdit(this)),
editButton(new QPushButton("Edit Profile", this)),
saveButton(new QPushButton("Save Changes", this))

//------- Layout of the page !

{
	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(25, 25, 25, 25); // Pad for the edges 
	mainLayout->setSpacing(16); // for spaces in between 


	auto* title = new QLabel("Profile", this);
	title->setObjectName("pageTitle");

	auto* description = new QLabel
	("View and update your account information.", this);
	description->setObjectName("pageDescription");

	mainLayout->addWidget(title);
	mainLayout->addWidget(description);
	mainLayout->addSpacing(20);

	//------DataBase needed later ;p
	usernameField->setText("Testing User"); // hardcode for now till real database is up 
	emailField->setText("helloteam7@gmail.com");

	auto* usernameLabel = new QLabel("Username:", this);
	auto* emailLabel = new QLabel("Email:", this);

	// username / password lable layout
	auto* usernameRow = new QHBoxLayout;  // username and a empty textbox side by side 
	usernameRow->addWidget(usernameLabel);
	usernameRow->addWidget(usernameField, 1); // the 1 so that the field can stretch to fill out empty spaces 
	auto* emailRow = new QHBoxLayout; // same idea 
	emailRow->addWidget(emailLabel);
	emailRow->addWidget(emailField, 1);

	//turn both username and email into a vertical layout 
	mainLayout->addLayout(usernameRow); 
	mainLayout->addLayout(emailRow);
	mainLayout->addSpacing(20);

	// row llayout for the ubttons 
	auto* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(); // pushes buttons to the right
	buttonLayout->addWidget(editButton);
	buttonLayout->addWidget(saveButton);
	mainLayout->addLayout(buttonLayout);
	mainLayout->addStretch(); // pushes everything upward to leave empty space at the bottom

	// readonly when the page first load in 
	setFieldsEditable(false);
	saveButton->setVisible(false);
	
	//--- connection for edit and save button ----- 
	connect(
		editButton,
		&QPushButton::clicked,
		this,
		&ProfilePage::toggleEditMode);
	connect(
		saveButton,
		&QPushButton::clicked,
		this,
		&ProfilePage::saveChanges);
}

	void ProfilePage::setFieldsEditable(bool editable) // switches for editing section to either read only or editable 
	{
		usernameField->setReadOnly(!editable);
		emailField->setReadOnly(!editable);

	}

	void ProfilePage::toggleEditMode()
	{
		isEditing = true;
		setFieldsEditable(true); 
		editButton->setVisible(false);
		saveButton->setVisible(true);
	}
	
	// ---- database needed ----- !!!!!!!!!!!!!!!!
	void ProfilePage::saveChanges()
	{
		QMessageBox::information // databbase goes here later, hardcode for testing 
		(
			this,
			"Profile Updated",
			"Your changes have been saved."
		);

		isEditing = false;
		setFieldsEditable(false);
		editButton->setVisible(true);
		saveButton->setVisible(false);
	}


