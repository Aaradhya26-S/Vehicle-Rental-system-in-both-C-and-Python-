import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import messagebox, simpledialog
import json, os
from datetime import*
FILE = "vehicles.json"
BOOK = "bookings.json"
ADMIN_PASS = "admin123"

# ---------- FILE ----------
def load(f): #read data from file 
    if not os.path.exists(f):
        return []
    return json.load(open(f))

def save(f, d): # write data to file
    json.dump(d, open(f, "w"), indent=4)

# ---------- DASHBOARD ----------
def dashboard(): # show total vehicles, rented vehicles and revenue
    data = load(FILE)
    rented = sum(1 for v in data if not v["available"])
    total = len(data)
    revenue = sum(v.get("revenue", 0) or 0 for v in data)

    text.delete("1.0", tk.END)
    text.insert(tk.END, f"""
                 DASHBOARD
-------------------------
Total Vehicles: {total}
Rented Vehicles: {rented}
Revenue: ₹{revenue}
""")
    

# ---------- ADD ----------
def add_vehicle(): # add new vehicle to system
    vid = simpledialog.askstring("ID", "Vehicle ID")
    name = simpledialog.askstring("Name", "Vehicle Name")
    price = simpledialog.askfloat("Price", "Price/day")

    if not vid or not name or price is None:
        messagebox.showerror("Error", "Invalid input!")
        return

    data = load(FILE)

    data.append({
        "id": vid,
        "name": name,   
        "price": price,
        "available": True,
        "revenue": 0
    })

    save(FILE, data)
    messagebox.showinfo("Done", "Vehicle Added")

# ---------- EDIT ----------
def edit_vehicle(): # edit price of existing vehicle
    vid = simpledialog.askstring("Edit", "Enter ID")
    data = load(FILE)

    for v in data:
        if v["id"] == vid:
            new_price = simpledialog.askfloat("New Price", "Enter new price")
            if new_price:
                v["price"] = new_price
                save(FILE, data)
                messagebox.showinfo("Updated", "Vehicle Updated")
            return

# ---------- DELETE ----------
def delete_vehicle(): # delete vehicle from system
    vid = simpledialog.askstring("Delete", "Enter ID")
    data = load(FILE)

    data = [v for v in data if v["id"] != vid]
    save(FILE, data)
    messagebox.showinfo("Deleted", "Vehicle Removed")

# ---------- DISPLAY ----------
def display(): # display all vehicles with details
    data = load(FILE)
    text.delete("1.0", tk.END)

    for v in data:
        status = " Available" if v["available"] else "Rented"
        text.insert(tk.END, f"{v['id']} | {v['name']} | ₹{v['price']} | {status}\n")
        print("loaded data:", load(FILE))  # DEBUG


# ---------- SEARCH ----------
def search():#helps find vehicles by name
    key = simpledialog.askstring("Search", "Enter name")
    data = load(FILE)
    text.delete("1.0", tk.END)

    for v in data:
        if key and key.lower() in v["name"].lower():
            text.insert(tk.END, f"{v}\n")

# ---------- SMART PRICING ----------
def get_price(price): # increase price by 20% on weekends
    if datetime.now().weekday() >= 5:
        return price * 1.2
    return price

# ---------- RENT ----------
def rent(): # rent a vehicle to customer and mark it unavailable
    vid = simpledialog.askstring("Rent", "Vehicle ID")
    name = simpledialog.askstring("Customer", "Name")
    days = simpledialog.askinteger("Days", "Number of rental days")

    data = load(FILE)

    for v in data:
        if v["id"] == vid and v["available"]:
            v["available"] = False
            v["customer"] = name
            v["days"] = days
            v["date"] = str(datetime.now().date())

            save(FILE, data)
            messagebox.showinfo("Done", f"Vehicle Rented for {days} days")
            return

    messagebox.showerror("Error", "Vehicle not available!")

# ---------- RETURN ----------
def return_v(): # return a vehicle, calculate bill and mark it available
    vid = simpledialog.askstring("Return", "Vehicle ID")
    data = load(FILE)

    for v in data:
        if v["id"] == vid and not v["available"]:

            days = v.get("days", 1) or 1
            price = get_price(v["price"])
            cost = days * price

            extra_days = simpledialog.askinteger("Late", "Extra days", initialvalue=0) or 0
            late_fee = extra_days * 500

            damage = simpledialog.askfloat("Damage", "Damage cost", initialvalue=0) or 0

            total = cost + late_fee + damage
            messagebox.showinfo("Bill", f"Total ₹{total:.2f}")


            v["available"] = True
            v["revenue"] = v.get("revenue", 0) + total

            save(FILE, data)
                 # ---------- PROFESSIONAL BILL ----------
            from datetime import datetime
            bill = f"""
=================  RENTAL INVOICE =================

Customer Name : {v.get('customer')}
Vehicle       : {v.get('name')}
Vehicle ID    : {v.get('id')}

----------------------------------------------------
Rental Details:
----------------------------------------------------
Days Rented   : {days}
Extra Days    : {extra_days}

Price/Day     : ₹{price:.2f}

----------------------------------------------------
Charges:
----------------------------------------------------
Base Rent     : ₹{cost:.2f}
Late Fee      : ₹{late_fee:.2f}
Damage Cost   : ₹{damage:.2f}

----------------------------------------------------
TOTAL AMOUNT  : ₹{total:.2f}
----------------------------------------------------

Date          : {datetime.now().strftime("%d-%m-%Y")}
Time          : {datetime.now().strftime("%H:%M:%S")}

====================================================
         Thank You for choosing our service 
====================================================
"""
import tkinter as tk
from tkinter import messagebox, simpledialog


# -------- FRAUD --------
def fraud_check(name): # check if customer has rented more than 2 vehicles (suspicious)
    data = load(FILE)
    count = sum(1 for v in data if v.get("customer") == name)

    if count > 2:
        messagebox.showwarning("Fraud Alert", "Suspicious User!")


# ---------- BOOK ----------
def book(): # customer can book a vehicle online, request goes to admin for approval
    vid = simpledialog.askstring("Book", "Vehicle ID")
    name = simpledialog.askstring("Name", "Customer")

    fraud_check(name)

    b = load(BOOK)
    b.append({"id": vid, "name": name, "status": "Pending"})
    save(BOOK, b)

    messagebox.showinfo("Done", "Request Sent")

# ---------- APPROVE ----------
def approve():
    b = load(BOOK)
    v = load(FILE)

    for req in b:
        if req["status"] == "Pending":
            for veh in v:
                if veh["id"] == req["id"] and veh["available"]:
                    veh["available"] = False
                    req["status"] = "Approved"

                    save(FILE, v)
                    save(BOOK, b)

                    messagebox.showinfo("Approved", req["name"])
                    return

            messagebox.showerror("Error", "Vehicle not available!")
            return

    messagebox.showerror("Error", "Vehicle not found!")

# ---------- FRAUD ----------
def fraud_check(name):
    data = load(FILE)
    count = sum(1 for v in data if v.get("customer") == name)
    if count > 2:
        messagebox.showwarning("Fraud Alert", "Suspicious User!")

# ---------- BOOK ----------
def book():
    vid = simpledialog.askstring("Book", "Vehicle ID")
    name = simpledialog.askstring("Name", "Customer")

    fraud_check(name)

    b = load(BOOK)
    b.append({"id": vid, "name": name, "status": "Pending"})
    save(BOOK, b)

    messagebox.showinfo("Done", "Request Sent")

# ---------- APPROVE ----------
def approve(): # admin approves booking request, marks vehicle unavailable
    b = load(BOOK)
    v = load(FILE)

    for req in b:
        if req["status"] == "Pending":
            for veh in v:
                if veh["id"] == req["id"] and veh["available"]:
                    veh["available"] = False
                    req["status"] = "Approved"

                    save(FILE, v)
                    save(BOOK, b)

                    messagebox.showinfo("Approved", req["name"])
                    return

# ---------- GRAPH ----------
def graph(): # show bar graph of revenue generated by each vehicle and pie chart of revenue distribution
    data = load(FILE)

    names = []
    revenue = []

    for v in data:
        names.append(v.get("name", "Unknown"))
        revenue.append(float(v.get("revenue", 0)))

    print("DEBUG DATA →")
    print("Names:", names)
    print("Revenue:", revenue)

    if not names:
        messagebox.showerror("Error", "No data to plot!")
        return

    if sum(revenue) == 0:
        messagebox.showwarning("Warning", "No revenue yet! Return vehicles first.")
        return
    #BAR GRAPH
        plt.figure(figsize=(8,5))
        plt.bar(names, revenue)
        plt.title("Revenue Generated by Vehicles")
        plt.xlabel("Vehicle Name")
        plt.ylabel("Revenue (₹)")
        plt.xticks(rotation=30)

        # Show values on top
        for i, v in enumerate(revenue):
            plt.text(i, v + 10, str(v), ha='center')
            plt.tight_layout()
            plt.show()

# PIE CHART (WITH LEGEND)
    if sum(revenue) > 0:
        plt.figure()
        clean_names = []
        clean_revenue = []
        for n, r in zip(names, revenue):
            if r > 0:
                clean_names.append(n)
                clean_revenue.append(r)
                wedges, _ = plt.pie(clean_revenue)

        plt.legend(
            wedges,
            [f"{n} - ₹{r}" for n, r in zip(clean_names, clean_revenue)],
            title="Vehicles",
            loc="center left",
            bbox_to_anchor=(1, 0.5)
        )

        plt.title("Revenue Distribution")
        plt.show()

# ---------- GUI ----------
root = tk.Tk() # main window, login system, buttons for each function, text area to display output 
root.title("Ultimate Rental System")
root.geometry("800x600")
root.config(bg="#121212")

login_frame = tk.Frame(root, bg="#121212")
login_frame.pack()

tk.Label(login_frame, text="LOGIN", fg="white", bg="#121212",
         font=("Arial", 18)).pack(pady=20)

entry = tk.Entry(login_frame, show="*")
entry.pack()

def login(): # simple password check to access main system
    if entry.get() == ADMIN_PASS:
        login_frame.pack_forget()
        main.pack()
    else:
        messagebox.showerror("Error", "Wrong Password")

tk.Button(login_frame, text="Login", command=login,
          bg="#1a0dad", fg="brown").pack(pady=10)

main = tk.Frame(root, bg="#121212")
btn_frame = tk.Frame(main, bg="#850606")
btn_frame.pack(pady=10)

buttons = [
    ("Dashboard", dashboard),
    ("Add Vehicle", add_vehicle),
    ("Edit Vehicle", edit_vehicle),
    ("Delete Vehicle", delete_vehicle),
    ("Display", display),
    ("Search", search),
    ("Rent", rent),
    ("Return", return_v),
    ("Online Booking", book),
    ("Approve Booking", approve),
    ("Graph", graph),
    ("Exit", root.quit)
]

row = 0
col = 0

for text_btn, cmd in buttons:
    tk.Button(btn_frame, text=text_btn, command=cmd,
              bg="#070B89", fg="white", width=20).grid(row=row, column=col, padx=5, pady=5)

    col += 1
    if col == 4:
        col = 0
        row += 1

text = tk.Text(main, height=15, width=90, bg="#1e1e1e", fg="white")
text.pack(pady=10)

root.mainloop()