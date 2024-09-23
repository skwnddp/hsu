from tkinter import *
from PIL import Image, ImageTk
import pyautogui
import pytesseract
from tkinter import filedialog

class CropApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Crop Image")

        self.canvas = Canvas(root, cursor="cross")
        self.canvas.pack(fill=BOTH, expand=YES)

        self.start_x = None
        self.start_y = None
        self.end_x = None
        self.end_y = None

        self.image = None

        self.canvas.bind("<ButtonPress-1>", self.start_crop)
        self.canvas.bind("<B1-Motion>", self.crop)
        self.canvas.bind("<ButtonRelease-1>", self.end_crop)

        self.btn_capture = Button(root, text="Capture Screen", command=self.capture_screen)
        self.btn_capture.pack()

        self.btn_save = Button(root, text="Save Cropped", command=self.save_cropped)
        self.btn_save.pack()

        self.btn_read_sort = Button(root, text="Read & Sort Numbers", command=self.read_and_sort)
        self.btn_read_sort.pack()

        self.text_display = Text(root, height=10, width=40)
        self.text_display.pack()

    def capture_screen(self):
        screenshot = pyautogui.screenshot()
        self.image = ImageTk.PhotoImage(screenshot)
        self.canvas.create_image(0, 0, anchor=NW, image=self.image)

    def start_crop(self, event):
        self.start_x = self.canvas.canvasx(event.x)
        self.start_y = self.canvas.canvasy(event.y)

    def crop(self, event):
        self.end_x = self.canvas.canvasx(event.x)
        self.end_y = self.canvas.canvasy(event.y)
        self.canvas.delete("crop_rect")
        self.canvas.create_rectangle(self.start_x, self.start_y, self.end_x, self.end_y, outline="red", tags="crop_rect")

    def end_crop(self, event):
        if self.end_x > self.start_x and self.end_y > self.start_y:
            self.crop_box = (self.start_x, self.start_y, self.end_x, self.end_y)

    def save_cropped(self):
        if self.crop_box and self.image:
            cropped_image = pyautogui.screenshot(region=self.crop_box)
            save_path = filedialog.asksaveasfilename(defaultextension=".png")
            cropped_image.save(save_path)

    def read_and_sort(self):
        if self.crop_box and self.image:
            cropped_image = pyautogui.screenshot(region=self.crop_box)
            text = pytesseract.image_to_string(cropped_image)
            numbers = [int(s) for s in text.split() if s.isdigit()]
            numbers.sort(reverse=True)
            sorted_numbers_str = '\n'.join(str(num) for num in numbers)
            self.text_display.delete(1.0, END)
            self.text_display.insert(END, sorted_numbers_str)

root = Tk()
app = CropApp(root)
root.mainloop()
