--[[
  Quest 335: [B Class] Wailing Cyclops Elimination
  Level Required: 56
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [333]
  Rewards: EXP=490174
]]

local fsm = require('fsm_engine')

local quest_335 = fsm:new({
    id = 335,
    name = "[B Class] Wailing Cyclops Elimination",
    level_required = 56,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {333},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 335: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 335: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 335: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 335: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 335: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 335: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 335: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 335: Rewards given")
                self:give_rewards({
                    exp = 490174,
                    items = {
                        { item_id = 241, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_335