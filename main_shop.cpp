#include <iostream>
#include <vector>
#include <string>

struct Product {
    int id;
    std::string name;
    double price;
};

class Cart {
    std::vector<Product> items;
public:
    void addProduct(const Product& p) {
        items.push_back(p);
        std::cout << "Добавлен в корзину: " << p.name << " (" << p.price << ")\n";
    }
    double total() const {
        double sum = 0;
        for (const auto& p : items) sum += p.price;
        return sum;
    }
    bool empty() const { return items.empty(); }
};

struct CustomerInfo {
    std::string name;
    std::string address;
    std::string phone;
};

class Order {
    Cart cart;
    CustomerInfo customer;
    bool paid = false;
    bool cancelled = false;
public:
    Order(const Cart& c, const CustomerInfo& info) : cart(c), customer(info) {}

    double getAmount() const { return cart.total(); }

    void markPaid() { paid = true; }
    void markCancelled() { cancelled = true; }
    bool isPaid() const { return paid; }
    bool isCancelled() const { return cancelled; }

    const CustomerInfo& getCustomer() const { return customer; }
};

class PaymentService {
public:
    bool pay(Order& order, double money) {
        std::cout << "Платёж: требуется " << order.getAmount()
                  << ", внесено " << money << "\n";
        if (money >= order.getAmount()) {
            std::cout << "Оплата прошла успешно.\n";
            order.markPaid();
            return true;
        } else {
            std::cout << "Недостаточно средств. Оплата отклонена.\n";
            return false;
        }
    }
};

class OrderProcessor {
public:
    void confirm(Order& order) {
        if (!order.isPaid()) {
            std::cout << "Нельзя подтвердить: заказ не оплачен.\n";
            return;
        }
        std::cout << "Заказ подтверждён для: " << order.getCustomer().name << "\n";
    }

    void processWarehouse(Order& order) {
        if (!order.isPaid()) {
            std::cout << "Нельзя обработать: заказ не оплачен.\n";
            return;
        }
        std::cout << "Заказ отправлен на склад для сборки.\n";
    }

    void ship(Order& order) {
        if (!order.isPaid()) {
            std::cout << "Нельзя отправить: заказ не оплачен.\n";
            return;
        }
        std::cout << "Заказ отправлен по адресу: " << order.getCustomer().address << "\n";
    }
};

int main() {
    std::vector<Product> catalog = {
        {1, "Книга C++", 30.0},
        {2, "Мышка", 15.0},
        {3, "Клавиатура", 25.0}
    };

    std::cout << " Каталог \n";
    for (auto& p : catalog) {
        std::cout << p.id << ") " << p.name << " - " << p.price << "\n";
    }


    Cart cart;
    cart.addProduct(catalog[0]);
    cart.addProduct(catalog[2]);
    std::cout << "Сумма корзины: " << cart.total() << "\n\n";

    if (cart.empty()) {
        std::cout << "Корзина пуста. Завершение.\n";
        return 0;
    }


    CustomerInfo info{"Иван", "г. Алмата, ул. Примерная 1", "+7-700-000-00-00"};
    Order order(cart, info);
    PaymentService payment;
    OrderProcessor processor;

    std::cout << " Сценарий 1: успешная оплата \n";
    bool ok = payment.pay(order, 60.0); 
    if (!ok) {
        std::cout << "Платёж не прошёл, заказ можно отменить или попробовать снова.\n";
        order.markCancelled();
    }

    if (order.isPaid()) {
        processor.confirm(order);
        processor.processWarehouse(order);
        processor.ship(order);
    }

    std::cout << "\n Сценарий 2: оплата не прошла \n";
    Order order2(cart, info);
    bool ok2 = payment.pay(order2, 10.0); 
    if (!ok2) {
        std::cout << "Клиент решает отменить заказ.\n";
        order2.markCancelled();
    }

    if (order2.isCancelled()) {
        std::cout << "Заказ отменён.\n";
    }

    return 0;
}