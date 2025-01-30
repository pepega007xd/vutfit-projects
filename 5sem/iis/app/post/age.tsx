import { Typography } from "@mui/material";

function format_duration(duration_ms: number): string {
  const seconds = Math.floor(duration_ms / 1000);
  const minutes = Math.floor(seconds / 60);
  const hours = Math.floor(minutes / 60);
  const days = Math.floor(hours / 24);
  const years = Math.floor(days / 365);

  if (years > 0) return `${years} year${years > 1 ? "s" : ""}`;
  if (days > 0) return `${days} day${days > 1 ? "s" : ""}`;
  if (hours > 0) return `${hours} hour${hours > 1 ? "s" : ""}`;
  if (minutes > 0) return `${minutes} min${minutes > 1 ? "s" : ""}`;
  return `${seconds} sec${seconds > 1 ? "s" : ""}`;
}

export default function Age({ age }: { age: Date; }) {
  const age_ms = Date.now() - new Date(age).getTime();

  return (
    <Typography color="textSecondary">{format_duration(age_ms)} ago</Typography>
  );
}
