#include <iostream>
#include <memory>
#include <string>

class RideContext; 

class RideState {
public:
    virtual ~RideState() = default;
    virtual const char* name() const = 0;

    virtual void selectCar(RideContext&, const std::string&) {
        deny("selectCar");
    }
    virtual void confirmOrder(RideContext&) {
        deny("confirmOrder");
    }
    virtual void carArrived(RideContext&) {
        deny("carArrived");
    }
    virtual void startTrip(RideContext&) {
        deny("startTrip");
    }
    virtual void finishTrip(RideContext&) {
        deny("finishTrip");
    }
    virtual void pay(RideContext&) {
        deny("pay");
    }
    virtual void cancel(RideContext&) {
        deny("cancel");
    }
    virtual void delay(RideContext&) {
        deny("delay");
    }
    virtual void paymentFailed(RideContext&) {
        deny("paymentFailed");
    }

protected:
    void deny(const std::string& action) const {
        std::cout << "Действие '" << action
                  << "' недоступно в состоянии '" << name() << "'\n";
    }
};

class IdleState;
class CarSelectedState;
class OrderConfirmedState;
class CarArrivedState;
class InTripState;
class TripCompletedState;
class TripCancelledState;


class RideContext {
    std::shared_ptr<RideState> state;
    std::string car;
    double fare = 0;
    bool isPaid = false;
public:
    RideContext(std::shared_ptr<RideState> init) : state(std::move(init)) {
        showState();
    }

    void setState(const std::shared_ptr<RideState>& s) {
        state = s;
        showState();
    }
    void showState() {
        std::cout << "[STATE] -> " << state->name() << "\n";
    }


    void setCar(const std::string& c) { car = c; }
    const std::string& getCar() const { return car; }
    void setFare(double f) { fare = f; }
    double getFare() const { return fare; }
    void setPaid(bool p) { isPaid = p; }
    bool paid() const { return isPaid; }


    void selectCar(const std::string& c)    { state->selectCar(*this, c); }
    void confirmOrder()                    { state->confirmOrder(*this); }
    void carArrived()                      { state->carArrived(*this); }
    void startTrip()                       { state->startTrip(*this); }
    void finishTrip()                      { state->finishTrip(*this); }
    void pay()                             { state->pay(*this); }
    void cancel()                          { state->cancel(*this); }
    void delay()                           { state->delay(*this); }
    void paymentFailed()                   { state->paymentFailed(*this); }
};



class IdleState : public RideState {
public:
    const char* name() const override { return "Idle"; }
    void selectCar(RideContext& ctx, const std::string& car) override;
    void cancel(RideContext& ctx) override;
};

class CarSelectedState : public RideState {
public:
    const char* name() const override { return "CarSelected"; }
    void selectCar(RideContext& ctx, const std::string& car) override;
    void confirmOrder(RideContext& ctx) override;
    void cancel(RideContext& ctx) override;
};

class OrderConfirmedState : public RideState {
public:
    const char* name() const override { return "OrderConfirmed"; }
    void carArrived(RideContext& ctx) override;
    void cancel(RideContext& ctx) override;
    void delay(RideContext& ctx) override;
};

class CarArrivedState : public RideState {
public:
    const char* name() const override { return "CarArrived"; }
    void startTrip(RideContext& ctx) override;
    void cancel(RideContext& ctx) override;
};

class InTripState : public RideState {
public:
    const char* name() const override { return "InTrip"; }
    void finishTrip(RideContext& ctx) override;
};

class TripCompletedState : public RideState {
public:
    const char* name() const override { return "TripCompleted"; }
    void pay(RideContext& ctx) override;
    void paymentFailed(RideContext& ctx) override;
};

class TripCancelledState : public RideState {
public:
    const char* name() const override { return "TripCancelled"; }
};



void IdleState::selectCar(RideContext& ctx, const std::string& car) {
    ctx.setCar(car);
    ctx.setFare(100.0); 
    std::cout << "Выбран автомобиль: " << car << ", тариф: " << ctx.getFare() << "\n";
    ctx.setState(std::make_shared<CarSelectedState>());
}

void IdleState::cancel(RideContext& ctx) {
    std::cout << "Заказ отменён на этапе ожидания.\n";
    ctx.setState(std::make_shared<TripCancelledState>());
}

void CarSelectedState::selectCar(RideContext& ctx, const std::string& car) {
    std::cout << "Изменяем выбранный автомобиль на: " << car << "\n";
    ctx.setCar(car);
    ctx.setFare(110.0);
}

void CarSelectedState::confirmOrder(RideContext& ctx) {
    std::cout << "Заказ подтверждён. Машина выехала.\n";
    ctx.setState(std::make_shared<OrderConfirmedState>());
}

void CarSelectedState::cancel(RideContext& ctx) {
    std::cout << "Заказ отменён до подтверждения.\n";
    ctx.setState(std::make_shared<TripCancelledState>());
}

void OrderConfirmedState::carArrived(RideContext& ctx) {
    std::cout << "Автомобиль прибыл к пользователю.\n";
    ctx.setState(std::make_shared<CarArrivedState>());
}

void OrderConfirmedState::cancel(RideContext& ctx) {
    std::cout << "Заказ отменён до прибытия автомобиля.\n";
    ctx.setState(std::make_shared<TripCancelledState>());
}

void OrderConfirmedState::delay(RideContext&) {
    std::cout << "Автомобиль задерживается. Уведомляем пользователя.\n";
}

void CarArrivedState::startTrip(RideContext& ctx) {
    std::cout << "Поездка начата.\n";
    ctx.setState(std::make_shared<InTripState>());
}

void CarArrivedState::cancel(RideContext& ctx) {
    std::cout << "Заказ отменён, хотя машина уже приехала.\n";
    ctx.setState(std::make_shared<TripCancelledState>());
}

void InTripState::finishTrip(RideContext& ctx) {
    std::cout << "Поездка завершена. Ожидается оплата.\n";
    ctx.setState(std::make_shared<TripCompletedState>());
}

void TripCompletedState::pay(RideContext& ctx) {
    std::cout << "Оплата прошла успешно. Спасибо за поездку!\n";
    ctx.setPaid(true);
}

void TripCompletedState::paymentFailed(RideContext& ctx) {
    std::cout << "Ошибка оплаты. Попробуйте ещё раз или свяжитесь с поддержкой.\n";
    ctx.setPaid(false);
}

int main() {
    auto idle = std::make_shared<IdleState>();
    RideContext ride(idle);

    std::cout << "\n Сценарий 1: нормальная поездка \n";
    ride.selectCar("Toyota Prius");
    ride.confirmOrder();
    ride.delay();       
    ride.carArrived();
    ride.startTrip();
    ride.finishTrip();
    ride.pay();

    std::cout << "\n Сценарий 2: отмена до подтверждения \n";
    RideContext ride2(idle);
    ride2.selectCar("Kia Rio");
    ride2.cancel();

    std::cout << "\n Сценарий 3: машина приехала, но заказ отменили \n";
    RideContext ride3(idle);
    ride3.selectCar("Hyundai Solaris");
    ride3.confirmOrder();
    ride3.carArrived();
    ride3.cancel();

    return 0;
}