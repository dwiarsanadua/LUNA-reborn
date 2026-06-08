--[[
  Quest 2061: [Daily]The Nexus Survey (Beginner)
  Level Required: 30
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=25717
]]

local fsm = require('fsm_engine')

local quest_2061 = fsm:new({
    id = 2061,
    name = "[Daily]The Nexus Survey (Beginner)",
    level_required = 30,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2061: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2061: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2061: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 451,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2061: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2061: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2061: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2061: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2061: Rewards given")
                self:give_rewards({
                    exp = 25717,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2061