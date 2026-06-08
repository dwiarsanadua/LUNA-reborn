--[[
  Quest 2039: [Daily] A Crack in Space 2
  Level Required: 89
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=2215547
]]

local fsm = require('fsm_engine')

local quest_2039 = fsm:new({
    id = 2039,
    name = "[Daily] A Crack in Space 2",
    level_required = 89,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2039: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2039: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2039: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2039: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2039: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2039: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2039: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2039: Rewards given")
                self:give_rewards({
                    exp = 2215547,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2039