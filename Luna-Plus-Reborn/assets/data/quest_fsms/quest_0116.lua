--[[
  Quest 116: Green Thumb
  Level Required: 21
  NPC Start: 25, NPC Complete: 26
  Rewards: EXP=4481, Gold=750
]]

local fsm = require('fsm_engine')

local quest_116 = fsm:new({
    id = 116,
    name = "Green Thumb",
    level_required = 21,
    npc_start = 25,
    npc_complete = 26,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 116: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 116: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 116: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 116: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 116: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 116: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 26,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 116: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 116: Rewards given")
                self:give_rewards({
                    exp = 4481,
                    gold = 750,
                    items = {
                        { item_id = 55, count = 1 },
                        { item_id = 21000008, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_116