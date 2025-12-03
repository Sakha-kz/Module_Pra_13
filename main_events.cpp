#include <iostream>
#include <vector>
#include <string>
#include <optional>

enum class Role { Guest, Registered, Admin };

struct Event {
    int id;
    std::string title;
    std::string date;
    std::string place;
};

enum class BookingStatus { Active, Cancelled };

struct User {
    int id;
    std::string name;
    Role role;
};

struct Booking {
    int id;
    int userId;
    int eventId;
    BookingStatus status;
};

class EventSystem {
    std::vector<Event> events;
    std::vector<User> users;
    std::vector<Booking> bookings;
    int nextBookingId = 1;

public:
    EventSystem() {
        events.push_back({1, "C++ Workshop", "2025-12-10", "Hall A"});
        events.push_back({2, "GameDev Meetup", "2025-12-15", "Hall B"});
        events.push_back({3, "AI Conference", "2026-01-05", "Main Hall"});

        users.push_back({1, "GuestUser", Role::Guest});
        users.push_back({2, "Alice", Role::Registered});
        users.push_back({3, "Admin", Role::Admin});
    }

    const User* getUserById(int id) const {
        for (const auto& u : users) if (u.id == id) return &u;
        return nullptr;
    }

    void listEvents() const {
        std::cout << " Список мероприятий \n";
        for (const auto& e : events) {
            std::cout << "ID=" << e.id << " | " << e.title
                      << " | " << e.date << " | " << e.place << "\n";
        }
    }

    std::optional<Event> findEvent(int id) const {
        for (const auto& e : events) if (e.id == id) return e;
        return std::nullopt;
    }

    void addEvent(const User& user, const Event& e) {
        if (user.role != Role::Admin) {
            std::cout << "Только админ может добавлять мероприятия.\n";
            return;
        }
        events.push_back(e);
        std::cout << "Мероприятие добавлено: " << e.title << "\n";
    }

    void deleteEvent(const User& user, int eventId) {
        if (user.role != Role::Admin) {
            std::cout << "Только админ может удалять мероприятия.\n";
            return;
        }
        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->id == eventId) {
                std::cout << "Удалено мероприятие: " << it->title << "\n";
                events.erase(it);
                return;
            }
        }
        std::cout << "Мероприятие не найдено.\n";
    }

    void bookEvent(const User& user, int eventId) {
        if (user.role == Role::Guest) {
            std::cout << "Гость не может бронировать. Нужна регистрация.\n";
            return;
        }
        if (!findEvent(eventId)) {
            std::cout << "Мероприятие не найдено.\n";
            return;
        }
        bookings.push_back({nextBookingId++, user.id, eventId, BookingStatus::Active});
        std::cout << "Бронирование создано. ID=" << (nextBookingId - 1) << "\n";
    }

    void cancelBooking(const User& user, int bookingId) {
        for (auto& b : bookings) {
            if (b.id == bookingId) {
                if (user.role != Role::Admin && b.userId != user.id) {
                    std::cout << "Вы не можете отменить чужое бронирование.\n";
                    return;
                }
                if (b.status == BookingStatus::Cancelled) {
                    std::cout << "Бронирование уже отменено.\n";
                    return;
                }
                b.status = BookingStatus::Cancelled;
                std::cout << "Бронирование " << bookingId << " отменено.\n";
                return;
            }
        }
        std::cout << "Бронирование не найдено.\n";
    }

    void listAllBookings(const User& user) const {
        if (user.role != Role::Admin) {
            std::cout << "Только админ может просматривать все бронирования.\n";
            return;
        }
        std::cout << " Все бронирования \n";
        for (const auto& b : bookings) {
            std::cout << "BookingID=" << b.id
                      << " userId=" << b.userId
                      << " eventId=" << b.eventId
                      << " status=" << (b.status == BookingStatus::Active ? "Active" : "Cancelled")
                      << "\n";
        }
    }
};

int main() {
    EventSystem sys;

    const User* guest = sys.getUserById(1);
    const User* alice = sys.getUserById(2);
    const User* admin = sys.getUserById(3);

    sys.listEvents();

    std::cout << "\n Гость пробует забронировать \n";
    sys.bookEvent(*guest, 1);

    std::cout << "\n Зарегистрированный пользователь бронирует \n";
    sys.bookEvent(*alice, 1);
    sys.bookEvent(*alice, 2);

    std::cout << "\n Админ смотрит все брони \n";
    sys.listAllBookings(*admin);

    std::cout << "\n Пользователь отменяет своё бронирование (ID=1) \n";
    sys.cancelBooking(*alice, 1);

    std::cout << "\n Админ удаляет мероприятие ID=3 \n";
    sys.deleteEvent(*admin, 3);

    std::cout << "\n Финальное состояние бронирований \n";
    sys.listAllBookings(*admin);

    return 0;
}