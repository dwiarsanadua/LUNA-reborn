--[[
  Quest 340: [B Class] Rageful Cyclops Elimination
  Level Required: 59
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [339]
  Rewards: EXP=619384
]]

local fsm = require('fsm_engine')

local quest_340 = fsm:new({
    id = 340,
    name = "[B Class] Rageful Cyclops Elimination",
    level_required = 59,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {339},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 340: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 340: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 340: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 340: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 340: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 340: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 340: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 340: Rewards given")
                self:give_rewards({
                    exp = 619384,
                    items = {
                        { item_id = 246, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_340