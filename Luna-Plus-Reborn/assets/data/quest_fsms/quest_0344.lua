--[[
  Quest 344: [B Class] Dark Zombie Elimination
  Level Required: 53
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [343]
  Rewards: EXP=379975
]]

local fsm = require('fsm_engine')

local quest_344 = fsm:new({
    id = 344,
    name = "[B Class] Dark Zombie Elimination",
    level_required = 53,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {343},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 344: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 344: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 344: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 344: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 344: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 344: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 344: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 344: Rewards given")
                self:give_rewards({
                    exp = 379975,
                    items = {
                        { item_id = 250, count = 1 },
                        { item_id = 250, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_344