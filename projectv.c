#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------ COLORS ------------------ */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

/* ------------------ CONSTANTS ------------------ */
#define ADMIN_PASSWORD "admin123"

/* ------------------ STRUCTS ------------------ */
struct Vehicle {
    int id;
    char type[20];
    char name[50];
    char model[50];
    float pricePerDay;
    int available;   /* 1 = available, 0 = rented */
};

struct OnlineBooking {
    int requestID;
    int vehicleID;
    char customerName[50];
    char customerPhone[15];
    char bookingDate[15];
    int daysRequested;
    int approved;    /* 0 = pending, 1 = approved */
};

/* ------------------ PROTOTYPES ------------------ */
void adminLogin();
void addVehicle();
void displayVehicles();
void rentVehicle();
void returnVehicle();
void payfordamage();
void onlineBooking();
void approveBooking();
int generateBookingRequestID();
void clearInput();

/* ------------------ UTILS ------------------ */
void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void showBookings() {
    FILE *fp = fopen("bookings.dat", "rb");
    struct OnlineBooking b;

    if (!fp) {
        printf("No bookings found.\n");
        return;
    }

    printf("\n--- ALL BOOKINGS ---\n");
    while (fread(&b, sizeof(b), 1, fp)) {
        printf("Request ID: %d | Vehicle ID: %d | Name: %s | Days: %d | Approved: %d\n",
               b.requestID, b.vehicleID, b.customerName, b.daysRequested, b.approved);
    }

    fclose(fp);
}


/* ------------------ MAIN ------------------ */
int main() {
    int choice;
    adminLogin();

    do {
        printf(YELLOW "\n====== Vehicle Rental System ======\n" RESET);
        printf(CYAN "1. " RESET "Add New Vehicle\n");
        printf(CYAN "2. " RESET "Display All Vehicles\n");
        printf(CYAN "3. " RESET "Rent a Vehicle (Walk-in)\n");
        printf(CYAN "4. " RESET "Return a Vehicle\n");
        printf(CYAN "5. " RESET "Record Damage Payment\n");
        printf(CYAN "6. " RESET "Online Booking Request (Customer)\n");
        printf(CYAN "7. " RESET "Approve Online Booking (Admin)\n");
        printf(CYAN "8. " RESET "Exit\n");
        printf(BLUE "Enter your choice: " RESET);
        if (scanf("%d", &choice) != 1) {
            clearInput();
            printf(RED "Invalid input! Please enter a number.\n" RESET);
            choice = -1;
            continue;
        }
        clearInput();

        switch (choice) {
            case 1: addVehicle(); break;
            case 2: displayVehicles(); break;
            case 3: rentVehicle(); break;
            case 4: returnVehicle(); break;
            case 5: payfordamage(); break;
            case 6: onlineBooking(); break;
            case 7: approveBooking(); break;
            case 8: printf(GREEN "Exiting... Goodbye!\n" RESET); break;
            default: printf(RED "Invalid choice!\n" RESET);
        }
    } while (choice != 8);

    return 0;
}

/* ------------------ ADMIN LOGIN ------------------ */
void adminLogin() {
    char pass[64];
    printf(BLUE "Enter admin password: " RESET);
    if (scanf("%63s", pass) != 1) {
        clearInput();
        printf(RED "Failed to read password. Exiting.\n" RESET);
        exit(1);
    }
    clearInput();

    if (strcmp(pass, ADMIN_PASSWORD) != 0) {
        printf(RED "Incorrect password! Access denied.\n" RESET);
        exit(0);
    }
    printf(GREEN "Login successful!\n" RESET);
}

/* ------------------ ADD VEHICLE ------------------ */
void addVehicle() {
    struct Vehicle v;
    FILE *fp = fopen("vehicles.dat", "ab");
    if (!fp) {
        printf(RED "Error opening vehicles.dat for writing!\n" RESET);
        return;
    }

    printf("\n" BLUE "Enter Vehicle ID: " RESET);
    if (scanf("%d", &v.id) != 1) { clearInput(); printf(RED "Invalid ID.\n" RESET); fclose(fp); return; }
    clearInput();

    printf(BLUE "Enter Vehicle Type (Car/Bike/Auto etc): " RESET);
    scanf("%19s", v.type); clearInput();

    printf(BLUE "Enter Vehicle Name: " RESET);
    scanf("%49s", v.name); clearInput();

    printf(BLUE "Enter Model: " RESET);
    scanf("%49s", v.model); clearInput();

    printf(BLUE "Enter Price Per Day: " RESET);
    if (scanf("%f", &v.pricePerDay) != 1) { clearInput(); printf(RED "Invalid price.\n" RESET); fclose(fp); return; }
    clearInput();

    v.available = 1;

    fwrite(&v, sizeof(v), 1, fp);
    fclose(fp);

    printf(GREEN "Vehicle Added Successfully!\n" RESET);
}

/* ------------------ DISPLAY VEHICLES ------------------ */
void displayVehicles() {
    struct Vehicle v;
    FILE *fp = fopen("vehicles.dat", "rb");
    if (!fp) {
        printf(YELLOW "No vehicles found!\n" RESET);
        return;
    }

    printf("\n" MAGENTA "--- Vehicle List ---\n" RESET);
    while (fread(&v, sizeof(v), 1, fp)) {
        printf(CYAN "ID: " RESET "%d | " CYAN "Type: " RESET "%s | " CYAN "Name: " RESET "%s | " CYAN "Model: " RESET "%s | " CYAN "Price/Day: " RESET "₹%.2f | " CYAN "Status: " RESET "%s\n",
               v.id, v.type, v.name, v.model, v.pricePerDay, v.available ? "Available" : "Rented");
    }
    fclose(fp);
}

/* ------------------ RENT VEHICLE (WALK-IN) ------------------ */
void rentVehicle() {
    struct Vehicle v;
    int id, days;
    char custName[50], rentDate[15], returnDate[15];
    int found = 0;

    FILE *fp = fopen("vehicles.dat", "rb+");
    if (!fp) {
        printf(RED "No vehicle records found!\n" RESET);
        return;
    }

    printf("\n" BLUE "Enter Vehicle ID to Rent: " RESET);
    if (scanf("%d", &id) != 1) { clearInput(); printf(RED "Invalid ID.\n" RESET); fclose(fp); return; }
    clearInput();

    while (fread(&v, sizeof(v), 1, fp)) {
        if (v.id == id) {
            found = 1;
            if (v.available == 0) {
                printf(RED "Vehicle is not available for rent.\n" RESET);
            } else {
                printf(BLUE "Enter Customer Name: " RESET);
                scanf("%49s", custName); clearInput();

                printf(BLUE "Enter Rent Date (dd/mm/yyyy): " RESET);
                scanf("%14s", rentDate); clearInput();

                printf(BLUE "Enter Return Date (dd/mm/yyyy): " RESET);
                scanf("%14s", returnDate); clearInput();

                printf(BLUE "Enter Days: " RESET);
                if (scanf("%d", &days) != 1) { clearInput(); printf(RED "Invalid days.\n" RESET); break; }
                clearInput();

                float total = days * v.pricePerDay;
                if (total > 5000.0f) {
                    printf(GREEN "10%% Discount Applied!\n" RESET);
                    total *= 0.9f;
                }

                printf(GREEN "Total Rent = ₹%.2f\n" RESET, total);

                v.available = 0;
                fseek(fp, - (long)sizeof(v), SEEK_CUR);
                fwrite(&v, sizeof(v), 1, fp);

                printf(GREEN "Vehicle Rented Successfully!\n" RESET);
            }
            break;
        }
    }

    if (!found) {
        printf(RED "Vehicle Not Found!\n" RESET);
    }

    fclose(fp);
}

/* ------------------ RETURN VEHICLE ------------------ */
void returnVehicle() {
    struct Vehicle v;
    int id, found = 0, lateDays = 0, damageCost = 0;
    float totalExtra = 0.0f;

    FILE *fp = fopen("vehicles.dat", "rb+");
    if (!fp) {
        printf(RED "No vehicle records found!\n" RESET);
        return;
    }

    printf("\n" BLUE "Enter Vehicle ID to Return: " RESET);
    if (scanf("%d", &id) != 1) { clearInput(); printf(RED "Invalid ID.\n" RESET); fclose(fp); return; }
    clearInput();

    while (fread(&v, sizeof(v), 1, fp)) {
        if (v.id == id) {
            found = 1;
            if (v.available == 1) {
                printf(YELLOW "Vehicle is not currently rented.\n" RESET);
            } else {
                printf(BLUE "Enter number of late days (0 if on time): " RESET);
                if (scanf("%d", &lateDays) != 1) { clearInput(); printf(RED "Invalid number.\n" RESET); break; }
                clearInput();

                if (lateDays > 0) {
                    totalExtra += lateDays * 200.0f;
                    printf(RED "Late Fee: ₹%.2f\n" RESET, lateDays * 200.0f);
                }

                printf(BLUE "Enter damage cost (0 if none): " RESET);
                if (scanf("%d", &damageCost) != 1) { clearInput(); printf(RED "Invalid amount.\n" RESET); break; }
                clearInput();

                totalExtra += (float)damageCost;

                if (totalExtra > 0.0f) {
                    printf(GREEN "Total extra charges = ₹%.2f\n" RESET, totalExtra);
                } else {
                    printf(GREEN "No extra charges.\n" RESET);
                }

                v.available = 1;
                fseek(fp, - (long)sizeof(v), SEEK_CUR);
                fwrite(&v, sizeof(v), 1, fp);

                printf(GREEN "Vehicle Returned Successfully!\n" RESET);
            }
            break;
        }
    }

    if (!found) printf(RED "Vehicle ID not found!\n" RESET);

    fclose(fp);
}

/* ------------------ RECORD DAMAGE PAYMENT ------------------ */
void payfordamage() {
    int id, cost, found = 0;
    struct Vehicle v;

    FILE *fp = fopen("vehicles.dat", "rb");
    if (!fp) {
        printf(RED "Error opening vehicles.dat!\n" RESET);
        return;
    }

    printf("\n" BLUE "Enter Vehicle ID: " RESET);
    if (scanf("%d", &id) != 1) { clearInput(); printf(RED "Invalid ID.\n" RESET); fclose(fp); return; }
    clearInput();

    while (fread(&v, sizeof(v), 1, fp)) {
        if (v.id == id) {
            found = 1;
            printf(BLUE "Enter Damage Amount: " RESET);
            if (scanf("%d", &cost) != 1) { clearInput(); printf(RED "Invalid amount.\n" RESET); break; }
            clearInput();

            printf(GREEN "Damage Payment ₹%d Recorded for %s (%s)\n" RESET, cost, v.name, v.model);
            break;
        }
    }

    if (!found) printf(RED "Vehicle Not Found!\n" RESET);

    fclose(fp);
}

/* ------------------ BOOKING ID GENERATOR ------------------ */
int generateBookingRequestID() {
    FILE *fp = fopen("bookings.dat", "rb");
    struct OnlineBooking b;
    int maxID = 1000;

    if (!fp) {
        return maxID + 1;
    }

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.requestID > maxID) maxID = b.requestID;
    }
    fclose(fp);
    return maxID + 1;
}

/* ------------------ ONLINE BOOKING (CUSTOMER) ------------------ */
void onlineBooking() {
    struct OnlineBooking b;
    FILE *fp = fopen("bookings.dat", "ab");
    if (!fp) {
        printf(RED "Error opening bookings.dat for writing!\n" RESET);
        return;
    }

    b.requestID = generateBookingRequestID();

    printf("\n" BLUE "Enter Vehicle ID to Book: " RESET);
    if (scanf("%d", &b.vehicleID) != 1) { clearInput(); printf(RED "Invalid ID.\n" RESET); fclose(fp); return; }
    clearInput();

    printf(BLUE "Enter Your Name: " RESET);
    scanf("%49s", b.customerName); clearInput();

    printf(BLUE "Enter Phone Number: " RESET);
    scanf("%14s", b.customerPhone); clearInput();

    printf(BLUE "Enter Booking Date (dd/mm/yyyy): " RESET);
    scanf("%14s", b.bookingDate); clearInput();

    printf(BLUE "Enter Days Needed: " RESET);
    if (scanf("%d", &b.daysRequested) != 1) { clearInput(); printf(RED "Invalid days.\n" RESET); fclose(fp); return; }
    clearInput();

    b.approved = 0;

    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);

    printf(GREEN "\nBooking Request Submitted!\n" RESET);
    printf(GREEN "Your Request ID = %d\n" RESET, b.requestID);
}

/* ------------------ APPROVE BOOKING (ADMIN) ------------------ */
void approveBooking() {
    struct OnlineBooking b;
    struct Vehicle v;

    FILE *fb = fopen("bookings.dat", "rb+");
    FILE *fv = fopen("vehicles.dat", "rb+");

    if (!fb || !fv) {
        if (!fb) printf(RED "No bookings found (bookings.dat missing)!\n" RESET);
        if (!fv) printf(RED "No vehicles found (vehicles.dat missing)!\n" RESET);
        if (fb) fclose(fb);
        if (fv) fclose(fv);
        return;
    }

    int reqID = 0;
    int foundReq = 0;

    printf("\n" BLUE "Enter Request ID to Approve: " RESET);
    if (scanf("%d", &reqID) != 1) { clearInput(); printf(RED "Invalid Request ID.\n" RESET); fclose(fb); fclose(fv); return; }
    clearInput();

    /* Find booking in file */
    while (fread(&b, sizeof(b), 1, fb)) {
        if (b.requestID == reqID) {
            foundReq = 1;
            break;
        }
    }

    if (!foundReq) {
        printf(RED "Request Not Found!\n" RESET);
        fclose(fb);
        fclose(fv);
        return;
    }

    if (b.approved == 1) {
        printf(YELLOW "Booking Already Approved!\n" RESET);
        fclose(fb);
        fclose(fv);
        return;
    }

    /* Check vehicle availability and update both files */
    rewind(fv);
    int foundVeh = 0;
    while (fread(&v, sizeof(v), 1, fv)) {
        if (v.id == b.vehicleID) {
            foundVeh = 1;
            if (v.available == 0) {
                printf(RED "Vehicle is Already Rented. Cannot Approve.\n" RESET);
                fclose(fb);
                fclose(fv);
                return;
            }

            float total = v.pricePerDay * b.daysRequested;

            printf(MAGENTA "\n--- Booking Approved ---\n" RESET);
            printf(CYAN "Customer: " RESET "%s\n", b.customerName);
            printf(CYAN "Vehicle: " RESET "%s (%s)\n", v.name, v.model);
            printf(CYAN "Days: " RESET "%d\n", b.daysRequested);
            printf(GREEN "Total Rent = ₹%.2f\n" RESET, total);

            /* update vehicle to rented */
            v.available = 0;
            fseek(fv, - (long)sizeof(v), SEEK_CUR);
            fwrite(&v, sizeof(v), 1, fv);

            /* update booking status to approved */
            b.approved = 1;
            fseek(fb, - (long)sizeof(b), SEEK_CUR);
            fwrite(&b, sizeof(b), 1, fb);

            printf(GREEN "\nBooking Approved Successfully!\n" RESET);
            break;
        }
    }

    if (!foundVeh) {
        printf(RED "Vehicle Not Found!\n" RESET);
    }

    fclose(fb);
    fclose(fv);
}
