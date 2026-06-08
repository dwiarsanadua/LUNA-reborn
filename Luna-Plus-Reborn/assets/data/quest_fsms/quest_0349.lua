--[[
  Quest 349: [B Class] Succubus Elimination
  Level Required: 60
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [347]
  Rewards: EXP=729940
]]

local fsm = require('fsm_engine')

local quest_349 = fsm:new({
    id = 349,
    name = "[B Class] Succubus Elimination",
    level_required = 60,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {347},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 349: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 349: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 349: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 349: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 349: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 349: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 349: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 349: Rewards given")
                self:give_rewards({
                    exp = 729940,
                    items = {
                        { item_id = 255, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_349