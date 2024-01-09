#pragma once
/**{
        std::vector<SDL_Rect> strips;

        SDL_Surface* image = IMG_Load(imagePath);
        if (image == nullptr)
            return strips;

        // Lock surface for direct pixel access
        if (SDL_LockSurface(image) == 0)
        {
            auto pixels = static_cast<uint32_t*>(image->pixels);
            int pitch = image->pitch / sizeof(uint32_t);

            // Flag to indicate whether we are currently inside a silhouette
            bool inSilhouette = false;

            // Iterate through each row of pixels
            for (int y = 0; y < image->h; ++y)
            {
                // Iterate through each column of pixels
                for (int x = 0; x < image->w; ++x)
                {
                    Uint32 pixel = pixels[y * pitch + x];

                    // Check if the pixel is transparent
                    if (pixel & 0xFF000000)
                    {
                        if (!inSilhouette)
                        {
                            // Start of a new silhouette, create a new strip
                            SDL_Rect stripRect;
                            stripRect.y = y;
                            stripRect.h = stripHeight;
                            stripRect.x = x;
                            stripRect.w = 1;
                            strips.push_back(stripRect);
                            inSilhouette = true;
                        }
                        else
                        {
                            // Expand the current strip
                            strips.back().w++;
                        }
                    }
                    else
                    {
                        inSilhouette = false;
                    }
                }

                // Reset silhouette flag at the end of each row
                inSilhouette = false;
            }

            // Unlock surface
            SDL_UnlockSurface(image);
        }

        // Free the surface
        SDL_FreeSurface(image);

        return strips;
    }
**/