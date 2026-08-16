import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// Project Pages URL: https://<user>.github.io/kactl/
// Override with VITE_BASE=/ for a custom domain or user site root.
export default defineConfig({
  plugins: [react()],
  base: process.env.VITE_BASE || '/kactl/',
})
