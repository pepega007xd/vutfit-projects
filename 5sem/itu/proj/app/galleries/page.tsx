// File: app/galleries/page.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13
import { UserGalleries } from './content';
import { get_current_user, SearchParams } from '@/actions';

// MyGalleries is a default galleries function that displays users own galleries.
// It reads the search parameters and calls UserGalleries component to display the galleries.
export default async function MyGalleries({ searchParams }: { searchParams: SearchParams }) {
  const search_string = (await searchParams).search as string ?? "";
  return (
    <UserGalleries user_id={(await get_current_user()).user_id} search_string={search_string} />
  );
}
