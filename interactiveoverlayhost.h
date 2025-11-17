#ifndef INTERACTIVEOVERLAYHOST_H
#define INTERACTIVEOVERLAYHOST_H

#include <QGraphicsScene>

/**
 * @brief Interface for components that can host interactive overlays.
 *
 * Provides access to the overlay scene that interactive components draw on.
 */
class InteractiveOverlayHost
{
public:
    virtual ~InteractiveOverlayHost() = default;

    /**
     * @brief Retrieve the overlay scene used for interactive elements.
     * @return Pointer to the overlay QGraphicsScene.
     */
    virtual QGraphicsScene* getOverlayScene() const = 0;
};

#endif // INTERACTIVEOVERLAYHOST_H

