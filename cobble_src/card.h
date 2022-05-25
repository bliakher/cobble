
#ifndef COBBLE_SRC_CARD_H
#define COBBLE_SRC_CARD_H

#include "image_loader.h"
#include <vector>
#include <map>

/**
 * Playing card containing images of symbols
 */
class Card {
public:
    /**
     * List of images on the card
     */
    std::vector<Image> Images_{};
    /**
     * Add new image to the list of images.
     * @param image Image to add
     */
    void AddImage(const Image& image);
    /**
     * Get the name of the common image between 2 cards.
     * @param card Card to compare
     * @return Name of the common image
     */
    std::string GetCommon(const Card* card);
private:


};

/**
 * Class representing a card rendered to the screen
 */
class RenderedCard {
public:
    RenderedCard() = default;
    /**
     * Rendered card has a circular form.
     * @param card Card to be rendered
     * @param centerX X coordinate of the center of the circle (card)
     * @param centerY Y coordinate of the center of the circle (card)
     * @param radius Circle (card) radius
     */
    RenderedCard(Card* card, int centerX, int centerY, int radius)
            : card_(card), centerX_(centerX), centerY_(centerY), radius_(radius) {}
    /**
     * Initialize card
     */
    void Init();
    /**
     * Render card to a renderer.
     * @param renderer Renderer
     */
    void Draw(SDL_Renderer *renderer);
    /**
     * Get image on card that was clicked from mouse coordinates.
     * @param mouseX X coordinate of the mouse
     * @param mouseY Y coordinate of the mouse
     * @return Clicked image or null (if nothing is clicked)
     */
    Image* GetClickedImage(int mouseX, int mouseY);
    /**
     * Get the name of the common image between 2 cards.
     * @param card Card to compare
     * @return Name of the common image
     */
    std::string GetCommon(const RenderedCard& card);
    /**
     * Get inner card object
     * @return Inner card
     */
    Card* GetCard();
private:
    /**
     * Degree increment for spacing images on cards
     */
    static const int DEGREE_INCREMENT = 110;
    Card* card_;
    int centerX_;
    int centerY_;
    int radius_;
    std::map<int, SDL_Rect> renderedImageBorders_{};
    int startDegree_;
    std::vector<int> imageRotations_{};
    void addImageBorder(int imageIdx, int imagePosX, int imagePosY, int width, int height);
};

/**
 * Deck of game cards
 *
 * In underlying structure, the deck of cards is represented by a projective plane.
 * Each card in the deck is represented by 1 line,
 * the points on that line are the images on the card.
 * This guarantees that every 2 cards have exactly 1 image in common.
 * Order of the plane (n) determines the size of the deck.
 * The deck will contain (n^2 + n + 1) cards with (n + 1) images on each card.
 */
class Deck {
public:
    Deck(): topCardIdx_(0) {}
    /**
     * Initialize deck.
     *
     * Create a projective plane of corresponding size. Use the plane to assign images to cards.
     * @param images List of all images in the game
     * @param imagesPerCard Number of images per card
     */
    void Init(const std::vector<Image>& images, int imagesPerCard);
    /**
     * Shuffle cards.
     *
     * Uses the Fisher–Yates shuffle
     */
    void Shuffle();
    /**
     * Open the top card from the deck
     * @return Card
     */
    Card* GetNextCard();
    /**
     * Get count of remaining (unopened) cards in the deck.
     * @return Number of remaining cards
     */
    int GetRemainingCardsCount();
    /**
     * Get total number of cards (opened and unopened).
     * @return Total number of cards
     */
    int GetTotalCardsCount();
private:
    std::vector<Card> cards_{};
    int topCardIdx_;

};

#endif //COBBLE_SRC_CARD_H
