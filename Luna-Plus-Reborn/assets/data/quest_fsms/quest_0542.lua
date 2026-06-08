--[[
  Quest 542: [Repeat][Quest] Fairy Wings
  Level Required: 150
  NPC Start: 99, NPC Complete: 99
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_542 = fsm:new({
    id = 542,
    name = "[Repeat][Quest] Fairy Wings",
    level_required = 150,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 542: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 542: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 542: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 542: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 542: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 542: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 542: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 542: Rewards given")
                self:give_rewards({
                    exp = 1000000,
                    items = {
                        { item_id = 338, count = 50 },
                        { item_id = 30000021, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_542