#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QDateEdit>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextEdit>
#include <QScrollArea>
#include <QLabel>
#include <QMap>
#include <QVector>
#include <QHeaderView>
#include <QFileDialog>

struct Expense {
    QString item;
    double amount;
    QString payer;
    QStringList participants;
    QDate date;
};

class ExpenseManager : public QMainWindow {
    Q_OBJECT

public:
    ExpenseManager(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        loadData();
    }

private slots:
    void createFriendCheckbox() {
        QWidget *friendWidget = new QWidget;
        QHBoxLayout *hbox = new QHBoxLayout(friendWidget);

        QCheckBox *payerCheckbox = new QCheckBox("Payer");
        payerCheckbox->setObjectName("payerCheckbox");
        QCheckBox *participantCheckbox = new QCheckBox("Participant");
        participantCheckbox->setObjectName("participantCheckbox");
        QLineEdit *nameEdit = new QLineEdit;
        nameEdit->setPlaceholderText("Friend Name");

        hbox->addWidget(payerCheckbox);
        hbox->addWidget(participantCheckbox);
        hbox->addWidget(nameEdit);
        friendsLayout->addWidget(friendWidget);
    }

    void addExpense() {
        bool ok;
        double amount = amountEdit->text().toDouble(&ok);
        QString item = itemEdit->text();
        QDate date = dateEdit->date();

        if (!ok || amount <= 0) {
            QMessageBox::critical(this, "Error", "Invalid amount");
            return;
        }
        if (item.isEmpty()) {
            QMessageBox::critical(this, "Error", "Item description required");
            return;
        }

        QString payer;
        QStringList participants;
        for (int i = 0; i < friendsLayout->count(); ++i) {
            QWidget *w = friendsLayout->itemAt(i)->widget();
            QCheckBox *payerCb = w->findChild<QCheckBox*>("payerCheckbox");
            QCheckBox *participantCb = w->findChild<QCheckBox*>("participantCheckbox");
            QLineEdit *le = w->findChild<QLineEdit*>();
            if (payerCb && payerCb->isChecked() && !le->text().isEmpty()) {
                payer = le->text();
            }
            if (participantCb && participantCb->isChecked() && !le->text().isEmpty()) {
                participants.append(le->text());
            }
        }

        if (payer.isEmpty() || participants.isEmpty()) {
            QMessageBox::critical(this, "Error", "Select at least one payer and participant");
            return;
        }

        expenses.append({item, amount, payer, participants, date});
        updateExpensesTable();
        calculateBalances();
        saveData();

        amountEdit->clear();
        itemEdit->clear();
    }

    void calculateBalances() {
        QMap<QString, double> balances;
        for (const Expense &exp : expenses) {
            double share = exp.amount / exp.participants.size();
            for (const QString &participant : exp.participants) {
                if (participant == exp.payer) continue;
                balances[participant] -= share;
                balances[exp.payer] += share;
            }
        }

        resultText->clear();
        resultText->append("Balances:");
        for (auto it = balances.begin(); it != balances.end(); ++it) {
            resultText->append(QString("%1: %2").arg(it.key()).arg(it.value(), 0, 'f', 2));
        }
    }

private:
    void setupUI() {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

        QWidget *friendsPanel = new QWidget;
        QVBoxLayout *friendsPanelLayout = new QVBoxLayout(friendsPanel);

        QPushButton *addFriendBtn = new QPushButton("Add Friend");
        connect(addFriendBtn, &QPushButton::clicked, this, &ExpenseManager::createFriendCheckbox);

        friendsPanelLayout->addWidget(addFriendBtn);

        QScrollArea *friendsScroll = new QScrollArea;
        QWidget *friendsWidget = new QWidget;
        friendsLayout = new QVBoxLayout(friendsWidget);
        friendsScroll->setWidget(friendsWidget);
        friendsScroll->setWidgetResizable(true);

        friendsPanelLayout->addWidget(friendsScroll);
        mainLayout->addWidget(friendsPanel);

        QWidget *inputPanel = new QWidget;
        QVBoxLayout *inputLayout = new QVBoxLayout(inputPanel);

        QLabel *amountLabel = new QLabel("Amount:");
        amountEdit = new QLineEdit;
        QLabel *itemLabel = new QLabel("Item:");
        itemEdit = new QLineEdit;
        QLabel *dateLabel = new QLabel("Date:");
        dateEdit = new QDateEdit(QDate::currentDate());
        QPushButton *addExpenseBtn = new QPushButton("Add Expense");
        connect(addExpenseBtn, &QPushButton::clicked, this, &ExpenseManager::addExpense);

        inputLayout->addWidget(amountLabel);
        inputLayout->addWidget(amountEdit);
        inputLayout->addWidget(itemLabel);
        inputLayout->addWidget(itemEdit);
        inputLayout->addWidget(dateLabel);
        inputLayout->addWidget(dateEdit);
        inputLayout->addWidget(addExpenseBtn);

        mainLayout->addWidget(inputPanel);

        expensesTable = new QTableWidget;
        expensesTable->setColumnCount(5);  // Updated to include date column
        expensesTable->setHorizontalHeaderLabels({"Item", "Payer", "Amount", "Participants", "Date"});
        expensesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        resultText = new QTextEdit;
        resultText->setReadOnly(true);

        QVBoxLayout *rightPanel = new QVBoxLayout;
        rightPanel->addWidget(expensesTable);
        rightPanel->addWidget(resultText);
        mainLayout->addLayout(rightPanel);
    }

    void updateExpensesTable() {
        expensesTable->setRowCount(expenses.size());
        for (int i = 0; i < expenses.size(); ++i) {
            const Expense &exp = expenses[i];
            expensesTable->setItem(i, 0, new QTableWidgetItem(exp.item));
            expensesTable->setItem(i, 1, new QTableWidgetItem(exp.payer));
            expensesTable->setItem(i, 2, new QTableWidgetItem(QString::number(exp.amount, 'f', 2)));
            expensesTable->setItem(i, 3, new QTableWidgetItem(exp.participants.join(", ")));
            expensesTable->setItem(i, 4, new QTableWidgetItem(exp.date.toString()));
        }
    }

    void saveData() {
        // Implement JSON-based data saving
    }

    void loadData() {
        // Implement JSON-based data loading
    }

    QVBoxLayout *friendsLayout;
    QTableWidget *expensesTable;
    QTextEdit *resultText;
    QLineEdit *amountEdit;
    QLineEdit *itemEdit;
    QDateEdit *dateEdit;
    QList<Expense> expenses;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ExpenseManager manager;
    manager.show();
    return app.exec();
}

#include "main.moc"
