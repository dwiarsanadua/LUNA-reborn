--[[
  Quest 343: [B Class] Dark Zombie Elimination
  Level Required: 53
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [341]
  Rewards: EXP=379975
]]

local fsm = require('fsm_engine')

local quest_343 = fsm:new({
    id = 343,
    name = "[B Class] Dark Zombie Elimination",
    level_required = 53,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {341},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 343: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 343: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 343: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 343: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 343: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 343: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 343: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 343: Rewards given")
                self:give_rewards({
                    exp = 379975,
                    items = {
                        { item_id = 249, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_343