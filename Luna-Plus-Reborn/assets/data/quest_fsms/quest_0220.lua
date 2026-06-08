--[[
  Quest 220: The History of The Continent
  Level Required: 150
  NPC Start: 5, NPC Complete: 49
  Rewards: EXP=910
]]

local fsm = require('fsm_engine')

local quest_220 = fsm:new({
    id = 220,
    name = "The History of The Continent",
    level_required = 150,
    npc_start = 5,
    npc_complete = 49,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 220: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 220: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 220: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 220: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 220: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 220: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 220: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 220: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 220: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 220: Rewards given")
                self:give_rewards({
                    exp = 910,
                    items = {
                        { item_id = 30000825, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_220