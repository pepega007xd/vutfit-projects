// File: app/galleries/content.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import * as React from 'react';
import ImageList from '@mui/material/ImageList';
import ImageListItem from '@mui/material/ImageListItem';
import { ImageListItemBar, Stack } from '@mui/material';
import { type Gallery, get_current_user, get_post, get_user, get_user_galleries, Subgallery } from '@/actions';
import { Gallery_header, New_Gallery_Button } from './gallery_components/components';

// A placeholder image for an empty gallery.
const placeholder = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAARMAAAC3CAMAAAAGjUrGAAAAM1BMVEUrOUhVYW0nNkVNWWZKVmNATVpSXmouPEsnNUU2Q1E6R1VTX2swPUwrOkhQXGlXY25EUV7WqELKAAACk0lEQVR4nO3b646qMBRA4ZZyVQTf/2lP5SZoiwxtwmGzvl9OZsKkS9ggEaUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiJFYZTIJeXn0UmJ5ZI2Oo3kevZY4kkq3kZJou6VKwq5SRQvSq45eUAS5Xcc9zWJI73ZbzdELCnezB44pZqMyQFkYe/jcjl5SqCS1b20xvC7LMnAaFHZr6dknSmJ023SLSKqnMSYryqDNNa02QVv4D9gmwxubtv2JIwt5m1+73emblEOTx3SJ0oZEkdSkGytjlICzqaQm9fwKw+zfUQQ1KbP5pWy7f3OCmiTPxaVovXtzl2yS3Jq1YoKaLI+d8RrOIblpnft/LanJ8oNg7p2xrySrUSQ1eV26TSPW+4GlT7IWRVITVdynJKlvTWOSlSiimqg6H0aK91bZO4nWvkErq4lSmWmaPPXeJ5snsXuKO4q0JnYphfoxXmdRnIePtCarPpNofXf+2YWaJLfv+9h3x55yoSbfe4lnplyniTuJ6+xzmSa+JI4oV2nimiW+s89Fmqwl+Rq0QpvUH6v0HjiuQSuzSZ3nj9mPv5J8RBHZpG7sKt9RfidZDlqJTYq8W+UYZUuSxaAV2GS8YzBEWR+vb+9BK69JPd1E6d76rUlslPHwEdekzhfHw7YDZ/jzIYq0JvMkr0H7hyRTFGFNlknsKv+SZIwiq8n7huxO3aAV1SQ4SR9FUpM6PEl39pHTpPycJTvldSmmSREnyesMLqWJiZXERjFCmkRGE5rQhCY0GdDk2/yLsZGc/vv2avN9o63O/1yGUrGeexsJeH6H57wceB7QJeJzo9nj9787hygPvvWOXgoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAeP4BHWQbwqZxv8wAAAAASUVORK5CYII=";

// Input values for a single gallery.
// Content is an array of either galleries ot subgalleries.
// Current_gallery_id is a unique ID for the currently displayed gallery and is used for redirecting to a subgallery.
// Display_edit if true will show buttons for gallery editing if it is false it will hide them.
type single_gallery_props = {
  content: (Gallery | Subgallery)[];
  current_gallery_id?: number;
  display_edit?: boolean;
}

// GalleriesList displays galleries ot subgalleries it recieves in the content variable.
export default async function GalleriesList({ content, current_gallery_id, display_edit }: single_gallery_props) {
  display_edit == undefined ? display_edit = true : 0;
  return (
    <ImageList
      variant="standard"
      cols={3}
      sx={{ px: "10vw" }}
    >
      {
        // Galleries or subgalleries inside the content variabel need to be displayed one at time, this is done by using a map() function.
        // reverse() is called so that the galleries appear in order from newest to oldest.
        await Promise.all(content.reverse().map(async (content) => {
          const gallery = content as Gallery;
          const subgallery = content as Subgallery;
          // && confirms that gallery.posts[0] has a valid value before the rest of the expression is called.
          const image_id = gallery.posts[0]
            && (await get_post(gallery.posts[0])).image_ids[0];

          const is_gallery = "gallery_id" in content;
          // First ImageListItem is an outer border and the second is an image link.
          return (
            <ImageListItem
              key={is_gallery ? gallery.gallery_id : subgallery.subgallery_id}
              sx={{
                border: '2px solid black',
                padding: '8px',
                borderRadius: '8px',
                display: 'flex',
                flexDirection: "column",
                justifyContent: 'center',
                alignItems: 'center',
                backgroundColor: '#2B2B2B',
                height: '26vh',
                width: '20vw',
              }}
            >
              <ImageListItem component="a"
                href={is_gallery
                  ? `/galleries/gallery/${gallery.gallery_id}`
                  : `/galleries/gallery/${current_gallery_id}/subgallery/${subgallery.subgallery_id}`}

              >
                <img
                  style={{
                    objectFit: "cover",
                    objectPosition: "center",
                    height: "24vh",
                    width: "18vw"
                  }}
                  className='w-full object-cover object-center h-full'
                  src={image_id
                    ? `/image/${image_id}`
                    : placeholder}
                  loading="lazy"
                />
              </ImageListItem>
              <ImageListItemBar position="below" title={is_gallery ? gallery.name : subgallery.name} />
            </ImageListItem>
          )
        }))
      }
    </ImageList>
  );
}

// DisplayGalleries puts together gallery header, buttons and list of galleries.
export async function DisplayGalleries({ galleries, only_public, header }: { galleries: Gallery[]; only_public: boolean; header: string }) {
  return (
    <Stack sx={{ justifyItems: "center", display: "flex", placeItems: "center", mx: "auto" }}>
      <Gallery_header header={header} display_edit={!only_public} />
      <Stack mx="auto" direction="row" spacing="3em" my="3em">
        {only_public ||
          <New_Gallery_Button />
        }
      </Stack>
      <GalleriesList content={galleries} />
    </Stack>
  );
}

// UserGalleries checks if the current user is the gallery owner, if not it gives DisplayGalleries only viewd users public galleries.
export async function UserGalleries({ user_id, search_string }: { user_id: number; search_string: string }) {
  const only_public = user_id !== (await get_current_user()).user_id;
  const user = await get_user(user_id);
  const galleries = await get_user_galleries(user_id, only_public, search_string);

  return (
    <DisplayGalleries
      galleries={galleries}
      only_public={only_public}
      header={`${user.username}'s galleries`}
    />
  )
}
