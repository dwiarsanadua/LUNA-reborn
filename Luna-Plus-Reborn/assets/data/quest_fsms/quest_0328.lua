--[[
  Quest 328: [B Class] Abandoned Giant Elimination
  Level Required: 50
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [325]
  Rewards: EXP=317098
]]

local fsm = require('fsm_engine')

local quest_328 = fsm:new({
    id = 328,
    name = "[B Class] Abandoned Giant Elimination",
    level_required = 50,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {325},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 328: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 328: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 328: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 96,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 328: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 328: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 328: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 328: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 328: Rewards given")
                self:give_rewards({
                    exp = 317098,
                    items = {
                        { item_id = 234, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_328