--[[
  Quest 882: Another Way
  Level Required: 95
  NPC Start: 129, NPC Complete: 129
  Rewards: EXP=7022796, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_882 = fsm:new({
    id = 882,
    name = "Another Way",
    level_required = 95,
    npc_start = 129,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 882: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 882: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 882: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 368,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 882: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 370,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 882: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 882: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 882: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 882: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 882: Rewards given")
                self:give_rewards({
                    exp = 7022796,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_882