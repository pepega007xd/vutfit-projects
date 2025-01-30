### Basic info

- App name: Artiscope
- Topic: Image sharing app with advanced organization features
- Tomáš Brablec (xbrabl04) - team leader
- Abas Hashmy (xhashm00)

# Overview

The goal is to create a web application for sharing images with advanced search and categorization features.

# Project plan

The applications will have five main pages between which the user can switch.

> **Division of work**: Each of the following pages is assigned to a single team member, that is specified after the page name.

## Home page (xbrabl04) - `app/post/*`, `app/page.tsx`

The Home page will contain a search bar, filter settings, and a list of posts. By default, the page will show a list of latest posts without any filtering. In the actual implementation, all advanced search and filtering options are hidden in a dropdown menu to clean up the user interface.

Each post will show the author's username and profile picture, post's name and description, a few images of the post, number of views and rating, and comments by other users. The post will also contain UI elements for adding a new comment, rating, and for storing the post into a gallery. Storing a post will involve a popup window for selecting the gallery or subgallery to store the post into (this popup is implemented by xhashm00 in `app/galleries/addbutton.tsx`). Each post can be viewed on a full page (described below).

## Create post page (xbrabl04) - `app/new_post/*`

This page will contain a list of images for the new post and text input for entering metadata. This is preferred over multipage design (where the user first selects images, then clicks "next" to enter the metadata), because the number of fields is not big, and it can be useful to see a preview of the images when entering the other information. In the final application, I decided to hide all optional inputs in a dropdown menu (see more in User testing (xbrabl04)).

For entering the metadata, there will be textboxes for post name and description, and an empty box for adding a tag. The page will also serve as a draft - when the user closes the page and opens it later, the filled in data will still be present. There will also be a button to discard all data.

## Public galleries page (xhashm00) `app/public/*`

This page contains all public galleries created by all users, it provides all the search options as other gallery pages but it doesn't allow the user to edit anything on this page. The only use case where editing on this page is possible is when user finds and opens his own gallery among the public ones.

## User galleries page (xhashm00) `app/galleries/*`, `app/user/*`

On this page, the user can create, view, edit, or delete his own galleries comprised of his own or other users' posts. User-owned galleries can be made private, preventing them from showing on the Public galleries page. Created galleries can also have subgalleries allowing further categorization. Subgalleries cannot be divided any further, this is to prevent users from creating large and difficult to navigate structures, but still providing a tool that allows effective categorization.

## Single post view (xbrabl04) - `app/post/*`

When opening a post either from galleries or from Home page, the post will be displayed on a full page, along with all its metadata (as described in Home page). If the current user is the author of the post, it will also be possible to edit the post's metadata (not the images themselves, instead of adding images to existing posts, the user will be encouraged to create a new post and use Galleries for organizing their posts).

# Deviation from original plan

## User collections (xhashm00)

User collections were redundant since users can just use their galleries for the same purpouse so they weren't implemented.

## User galleries (xhashm00)

Due to lack of time, gallery tags were not implemented.

# User Testing (xbrabl04)

## The Participant

The participant is a male college student (age group 20-26). He studies physics, which involves some amount of computer work - mainly data processing in Excel or Python, simulations in specialized software, etc. He is therefore proficient in general use of a computer and has very basic knowledge of software development. The student paints and draws in his free time and posts his work on Instagram.

## Tested Tasks

The participant was not given any additional information than the purpose of the application (which we discussed earlier when researching the User requirements). Then he was given the following tasks without any other hints or instructions:

- Create a post with multiple photos, add a title and post it.
- Find the post and add it to their public gallery (setting up the gallery itself was not a part of the task since galleries are not a part of my work, the point was only to test whether the user can find the button for adding a post to a gallery).
- add a description and a tag to the post.
- Return to main page and sort posts according to views (multiple other posts with more views were added before the test so that the new post wouldn't be immediately visible).
- Find the new post using the tag.
- Delete the post.

The user handled the tasks this way:

- When creating the post, he immediately found the "New Post" link in navigation and got to the page for creating the post. Then he uploaded images and added a title without a problem, but before creating the post, he asked whether he needs to fill out all the other information. Apparently, it is not obvious whether the fields can be left alone. In the GUI, the "Name" field is annotated with a star, but this is apparently not obvious at first sight. Therefore I decided to hide the other options behind a collapsable menu.

- Finding the post was simple, the user clicked the "Home" link and saw their own post immediately, since the posts are sorted from newest by default. However, adding the post to a gallery took him some time, according to him it was not obvious that the buttom with a series of images represented a gallery.

- Adding a description and a tag was easy for the user, after he figured out how to open the post in the "Single post view" using the button in the post header. Again, according to him, it wasn't clear the button would open the post for editing. It might be better to change the icon to a pencil to signify editing. However, any post can be opened in a single post view, but only the current user's posts can be edited - fixing this would require a larger redesign of the single post view.

- Returning to the homepage was confusing to the user - he expected there to be a dedicated "back" button. However, he figured out that he can simply click the browser's native "back" button to return to homepage. I do not really see a reason to add a dedicated back button in the app, when the app is built for desktop only. The user had no problems finding the extra sort and filter options in the top bar and using it to sort posts.

- The user first tried to find the post using the search bar (which worked, the search bar also searches through tags), but I told him specifically to look for a tag. After that, he used the tag filter correctly.

- The user had no problem opening the post again in a single post view and deleting it using the trash button. Apparently, the association between opening a single post and editing it is simple to pick up.

## Key takeaways

The first takeaway from the user testing was that icon buttons can be confusing when the user is not familiar with the meaning of the icon beforehand (we used Material Design icons, but when the user never used a Material Design UI, the icons can be confusing). This could be solved by adding text to the buttons themselves or as a hover tooltip.

The second takeaway is that it is not obvious which fields are required and which are optional in the form for creating a new post. I solved this issue in the final version by hiding the additional (optional) fields in a collapsable menu. This also made the form look less crowded.

# User Testing (xhashm00)

## The Participant

The Participant is a male soldier (age group 25-30). He sometimes works as 2D and 3D designer for which he uses advanced software like Autodesk Maya and similar programs.
He is therefore proficient in general software handling and also has some experience with programming and experience with working on large scale projects.

## Tested Tasks

The participant was not given any additional information than the purpose of the application (which we discussed earlier when researching the User requirements). Then he was given the following tasks without any other hints or instructions:

- Create a gallery and a subgallery and then rename them.
- Find posts you like and add them to your gallery or subgallery. The participant was told where the button to add a post to a gallery was but was not told how it works.
- Look at some publicly displayed galleries.
- Create a new gallery with a subgallery and then delete them.
- Remove a post from your gallery.
- Try to search for something in a gallery.
- Make your gallery private.
- View galleries of a specific user.

The user handled the tasks this way:

- The user had no problem creating a new gallery but he first tried to rename it without opening it and when that didn't work he realized that he had to open the gallery to rename it. He commented that being able to rename it on the page for all galleries would be more convenient but having to open it first made sense.

- There was no problem when the user tried to add a post he liked to a gallery or subgallery or when he tried to view public galleries of other users.

- When the user was asked to delete a subgallery he figured out that he need to open the subgallery to delete it but commented that it might be confusing for some users since they might confuse delete gallery button with delete subgallery button and expressed that there should be a warning before something is deleted.

- The user had no problem with removing a post from a gallery, with searching for something within a gallery or with making the gallery private.

- When asked to view galleries of a specific user he intuitively navigated to a post and clicked on the user icon which showed him that users public galleries but commented that it would be nice to also have a similar button in the users gallery itself.

## Key takeaways

The main takeaway is that users should be warned before something is permanently deleted. For this task a popup that prevents the user from doing anything else and draws their attention to that decision would be a good solution.

Another takeaway is that there should be a user profile button in their gallery instead of just on their posts so that other users that are viewing the gallery can find the creator.
