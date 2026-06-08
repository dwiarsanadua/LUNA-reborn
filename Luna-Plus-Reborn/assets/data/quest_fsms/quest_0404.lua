--[[
  Quest 404: [Quest]The Imperishable King
  Level Required: 62
  NPC Start: 90, NPC Complete: 90
  Prerequisites: [403]
  Rewards: EXP=633637
]]

local fsm = require('fsm_engine')

local quest_404 = fsm:new({
    id = 404,
    name = "[Quest]The Imperishable King",
    level_required = 62,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {403},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 404: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 404: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 404: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 79,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 404: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 404: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 404: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 404: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 404: Rewards given")
                self:give_rewards({
                    exp = 633637,
                    items = {
                        { item_id = 301, count = 1 },
                        { item_id = 30000038, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_404