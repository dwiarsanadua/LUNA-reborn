--[[
  Quest 320: [B Class] Basilisk Elimination
  Level Required: 46
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [319]
  Rewards: EXP=212963
]]

local fsm = require('fsm_engine')

local quest_320 = fsm:new({
    id = 320,
    name = "[B Class] Basilisk Elimination",
    level_required = 46,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {319},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 320: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 320: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 320: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 320: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 320: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 320: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 320: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 320: Rewards given")
                self:give_rewards({
                    exp = 212963,
                    items = {
                        { item_id = 226, count = 1 },
                        { item_id = 226, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_320