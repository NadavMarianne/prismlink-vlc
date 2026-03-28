# Color-Optical Communication

**A project exploring custom-designed communication protocols and assembling a new physical communication layer**

Created by: Nadav Marianne

---

## Table of Contents
* [Topic Description](#topic-description)
* [Theoretical Background](#theoretical-background)

---

## Theoretical Background

In the world of physics, colors are expressed as electromagnetic waves where their frequency falls within a certain range visible to the human eye; the difference in frequency causes the color to change. Humans have given names to colors whose frequencies are found in specific areas of the spectrum.

In the art world, it was established that there are 3 primary colors from which every other color can be composed (except black and white) by combining them. These colors are red, blue, and yellow.

In computer science, the concept from the art world—producing every color using only 3 base colors—was adopted, but adapted to the physics world by using the 3 basic light colors: red, green, and blue. 

These 3 colors are commonly referred to as **RGB**. This is a well-known term in digital computer graphics, which consist of pixels. Each pixel contains 3 sub-pixels arranged in a specific layout from left to right. In a standard RGB layout, red is on the left of the pixel, green is in the middle, and blue is on the right. *(Note: There are also screens and display systems that use other layouts like BGR, meaning blue on the left, green in the middle, and red on the right).*

In mathematics, taking inspiration from the digital graphics world, it was decided that every color would be represented by a specific value assigned to each of the RGB components. Following the standard layout, each component can receive a hexadecimal value in a 2-digit range (from `00` to `FF`, or 0 to 255 in decimal). A low value indicates minimal usage of that color component, and a high value indicates maximum usage.

For example, the colors black and white can be represented by the hex codes `#000000` and `#FFFFFF` respectively.
* **Red** is `#FF0000`
* **Green** is `#00FF00`
* **Blue** is `#0000FF`

For any other color we want to represent mathematically, we can combine these base channels and scale their intensity. In total, there are 16,777,216 different possible color combinations. Because our 3 color channels (RGB) each have 256 options (which is 2^8), the math works out to:

(2^8)^3 = 2^24

### The 8-Color Palette (3-Bit)

Mathematically, for a phone camera to easily, quickly, and accurately distinguish between colors, we want to use only the colors located at the absolute edges of the RGB spectrum. This means we take the 8 vertices (corners) of the color cube that the RGB format provides.

These colors are:
* **Black:** `#000000`
* **White:** `#FFFFFF`
* **Red:** `#FF0000`
* **Green:** `#00FF00`
* **Blue:** `#0000FF`
* **Cyan:** `#00FFFF`
* **Magenta (Pink):** `#FF00FF`
* **Yellow:** `#FFFF00`

Because we have exactly 8 colors, this means we will be operating in an octal base, meaning each color represents exactly **3 bits** of data.

### Parallel Communication & Screen Splitting

To increase the communication speed, we can utilize the physical size of the screen. In theory, screen size shouldn't matter as long as the camera can identify the phone. However, if we split the screen into sections, we can communicate over multiple channels simultaneously. By broadcasting multiple colors at once, we achieve parallel communication.

Current smartphone cameras allow us to comfortably split the screen horizontally so that other smartphones can receive the messages from a reasonable distance. 

Because most modern smartphone screens share roughly a 16:9 aspect ratio (meaning the height is almost double the width), we need to divide the screen intelligently. Since we are programming the core logic in C, we must prevent data waste and memory misalignment by working in complete **Bytes** (multiples of 8 bits). 

If we used a 3x3 grid, we would transfer 9 colors * 3 bits = 27 bits, which does not divide evenly into bytes. 
Instead, we will use a **2x4 screen division** (2 columns, 4 rows). This gives us 8 cells in total. 
8 cells * 3 bits = **24 bits per frame**. 
24 bits aligns perfectly into exactly 3 Bytes per frame.

### Hardware Limitations & Frame Rates

Most smartphone cameras today are capable of recording at either 30fps or 60fps, while modern screens can easily display images at a refresh rate of 60Hz or higher. Therefore, the receiving camera is our primary limiting factor. 

To solve this, our software will feature two distinct operating modes:
1. **30Hz Mode:** For standard reliability.
2. **60Hz Mode:** To allow users with advanced devices to utilize their camera's full capabilities and double the data reception rate.