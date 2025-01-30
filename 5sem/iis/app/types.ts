export type UserType = "administrator" | "moderator" | "user" | "guest";

export interface Comment {
  comment_id: number,
  comment_text: string,
  creation_time: Date,
  photo_id: number,
  author_id: number,
}

export interface Groups {
  group_id: number,
  group_name: string,
  description?: string,
  creation_time?: Date,
  owner_id: number,
}

export interface Photo {
  photo_id: number,
  name: string,
  description?: string,
  location?: string,
  image_id: number,
  is_private: boolean,
  creation_time: Date,
  author_id: number,
}

export interface Share {
  share_id: number,
  comment?: string,
  creation_time: Date,
  photo_id: number,
  sender_id: number,
  receiver_id: number,
}

export interface Users {
  user_id: number,
  username: string,
  full_name?: string,
  biography?: string,
  is_blocked: boolean,
  creation_time: Date,
  user_type: UserType,
  password_hash: string,
}

export interface Rating {
  is_positive: boolean,
}

export interface Post {
  photo: Photo,
  user: Users,
  tags: string[],
  comments: Comment[],

  positive_rating: number,
  negative_rating: number,

  user_rating?: boolean,
}

export interface Message {
  photo: Photo,
  sender: Users,
  share: Share,
}


export interface User_Group_Member {
  user_id: number,
  group_id: number
}
