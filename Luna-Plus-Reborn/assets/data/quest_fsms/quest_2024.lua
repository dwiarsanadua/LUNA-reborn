--[[
  Quest 2024: [Daily]A Gourment Request 4
  Level Required: 74
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=979401
]]

local fsm = require('fsm_engine')

local quest_2024 = fsm:new({
    id = 2024,
    name = "[Daily]A Gourment Request 4",
    level_required = 74,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2024: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2024: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2024: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2024: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2024: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2024: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2024: Rewards given")
                self:give_rewards({
                    exp = 979401,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2024