/***************************************************************************//**
 * @file list.h
 * @author Dorian Weber
 * @brief Header file of a singulary linked list.
 * @sa list.c
 ******************************************************************************/

#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/* forward declaration of opaque structure */
typedef struct s_list list;

/**@brief Creates a new list.
 * @return pointer to a new list
 */
extern list* newList();

/**@brief Frees the memory for the list structure.
 * @param[in] src  list to free
 */
extern void deleteList(list* src);

/**@brief Adds a new datum after the current position.
 * @param[in] src   list to append
 * @param[in] data  pointer to store 
 */
extern void listAdd(list* src, const void* data);

/**@brief Removes the currently selected item.
 * @pre A valid element is selected.
 * 
 * @param[in] src  list to remove from
 */
extern void listRemove(list* src);

/**@brief Sets the datum at the currently selected index.
 * @pre A valid element is selected.
 * 
 * @param[in] src   list to set
 * @param[in] data  pointer to store
 */
extern void listSet(list* src, const void* data);

/**@brief Returns the currently selected element.
 * @pre A valid element is selected.
 * 
 * @param[in] src  list to retrieve data from
 * @return data pointer
 */
extern void* listGet(list* src);

/**@brief Advances the list one position and returns 0, if the end was reached.
 * @param[in] src  list to advance
 */
extern int listNext(list* src);

/**@brief Sets the current element to the top.
 * @note This is always one element before the first one.
 * 
 * @param[in] src  list to reset
 */
extern void listStart(list* src);

/**@brief Counts the list.
 * @param[in] src  list to count
 * @return number of items currently stored
 */
extern int listCount(list* src);

/**@brief Returns \c 1, if this list is empty, else \c 0.
 */
extern int listIsEmpty(list* src);

#endif
