--[[
  Quest 773: [Party] ID'd
  Level Required: 32
  NPC Start: 29, NPC Complete: 29
  Rewards: EXP=47463, Gold=8704
]]

local fsm = require('fsm_engine')

local quest_773 = fsm:new({
    id = 773,
    name = "[Party] ID'd",
    level_required = 32,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 773: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 773: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 773: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 85,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 773: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 773: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 773: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 773: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 773: Rewards given")
                self:give_rewards({
                    exp = 47463,
                    gold = 8704,
                    items = {
                        { item_id = 380, count = 1 },
                        { item_id = 380, count = 1 },
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_773