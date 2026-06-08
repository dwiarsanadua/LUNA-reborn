--[[
  Quest 786: [Party] Jenebu
  Level Required: 47
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=166149, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_786 = fsm:new({
    id = 786,
    name = "[Party] Jenebu",
    level_required = 47,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 786: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 786: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 786: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 146,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 786: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 786: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 786: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 786: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 786: Rewards given")
                self:give_rewards({
                    exp = 166149,
                    gold = 7415,
                    items = {
                        { item_id = 21001505, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_786