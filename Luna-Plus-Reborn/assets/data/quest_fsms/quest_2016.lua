--[[
  Quest 2016: [Daily] Troll Warrior Subjugation
  Level Required: 66
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=424645
]]

local fsm = require('fsm_engine')

local quest_2016 = fsm:new({
    id = 2016,
    name = "[Daily] Troll Warrior Subjugation",
    level_required = 66,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2016: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2016: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2016: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 103,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2016: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2016: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2016: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2016: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2016: Rewards given")
                self:give_rewards({
                    exp = 424645,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2016